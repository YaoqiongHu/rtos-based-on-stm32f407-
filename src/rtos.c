#include "rtos.h"
#include "core.h"

queue* readyqueue[4];
queue* sleepqueue;
tcb*   pxCurrentTCB;

/* ===== 可嵌套临界区:保存/恢复 PRIMASK,内层不会误开外层临界区 ===== */
uint32 rtos_enter_critical(void)
{
    uint32 mask;
    __asm volatile("mrs %0, primask" : "=r"(mask));  /* 记下原状态 */
    __asm volatile("cpsid i");                        /* 关中断 */
    return mask;
}

void rtos_exit_critical(uint32 mask)
{
    __asm volatile("msr primask, %0" :: "r"(mask));  /* 恢复原状态,不强行打开 */
}

void rtos_init(void)
{
    tcbpool_init();
    stackpool_init();
    heap256pool_init();
    heap128pool_init();
    heap64pool_init();
    heap32pool_init();
    os_semaphore_init();
    readyqueue[0] = create_queue();
    readyqueue[1] = create_queue();
    readyqueue[2] = create_queue();
    readyqueue[3] = create_queue();
    sleepqueue = create_queue();

}

void task_exit(void)
{
    while(1);
}

tcb* rtos_create_task(void (*entry)(void),uint32 priority,uint32 pid)
{
    if (priority >= 4)
        return 0;                    /* 优先级越界(readyqueue 只有 4 条) */

    tcbnode* tk = tcb_alloc();
    if (tk == 0)
        return 0;
    tcb* task = (tcb*)tk->tcbptr;

    stacknode* st = stack_alloc();
    if (st == 0)
    {
        tcb_free(tk);                /* 栈没拿到,TCB 还回去 */
        return 0;
    }
    /* 铺伪造栈帧:顺序和 SVC/PendSV 弹栈完全对应 */
    uint32* sp = (uint32*)st->stackptr;
    *(--sp) = 0x01000000;            /* xPSR:必须置 Thumb 位(bit24) */
    *(--sp) = (uint32)entry;         /* PC:任务函数 */
    *(--sp) = (uint32)task_exit;     /* LR:任务返回兜底 */
    *(--sp) = 0;                     /* r12 */
    *(--sp) = 0;                     /* r3  */
    *(--sp) = 0;                     /* r2  */
    *(--sp) = 0;                     /* r1  */
    *(--sp) = 0;                     /* r0  */
    *(--sp) = 0;                     /* r11 */
    *(--sp) = 0;                     /* r10 */
    *(--sp) = 0;                     /* r9  */
    *(--sp) = 0;                     /* r8  */
    *(--sp) = 0;                     /* r7  */
    *(--sp) = 0;                     /* r6  */
    *(--sp) = 0;                     /* r5  */
    *(--sp) = 0;                     /* r4  */

    task->sp = (uint32)sp;           /* sp = 帧底(指向 r4),SVC 从这里弹 */

    task->tcb_hand     = tk;
    task->pid          = pid;
    task->priority     = priority;
    task->sleep        = 0;
    task->stack_hand   = st;
    heapnode* qh = heap32pool_alloc();
    if (qh == 0)                       /* heap32 池空:回滚已占用的 TCB/栈 */
    {
        stack_free(st);
        tcb_free(tk);
        return 0;
    }
    task->queuenode_hand = qh->heapptr;
    task->queuenode_hand->heap_hand = qh;
    task->queuenode_hand->tcbowner = task;
    task->queuenode_hand->queueowner = 0;
    queue_add(readyqueue[priority], task);

    if (pxCurrentTCB != 0 && priority < pxCurrentTCB->priority)
        SCB->ICSR |= SCB_ICSR_PENDSVSET;
    return task;
}

void vTaskSwitchContext(void)
{
    if(readyqueue[0]->head != 0)
    {
        pxCurrentTCB = queue_pop_head(readyqueue[0]);
    }
    else if(readyqueue[1]->head != 0)
    {
        pxCurrentTCB = queue_pop_head(readyqueue[1]);
    }
    else if(readyqueue[2]->head != 0)
    {
        pxCurrentTCB = queue_pop_head(readyqueue[2]);
    }
    else if(readyqueue[3]->head != 0)
    {
        pxCurrentTCB = queue_pop_head(readyqueue[3]);
    }
}

/* 调度器启动:中断配置 → 选第一个任务 → SVC 点火(不再返回) */
void rtos_start(void)
{
    systick_init();            /* PendSV/SysTick 最低优先级 + 1ms tick */
    vTaskSwitchContext();      /* 弹出最高优先级任务 */

    /* 关键:svc 之前把线程栈切到 PSP,并让 PSP 指向任务栈顶。
       这样 svc 进入异常时硬件自然生成 EXC_RETURN=0xFFFFFFF9(线程+PSP),
       SVC_Handler 的 bx lr 才会从任务帧弹栈。 */
    __asm volatile(
        "  mrs r0, control        \n"
        "  orr r0, r0, #2         \n"  /* SPSEL=1:线程模式改用 PSP */
        "  msr control, r0        \n"
        "  ldr r1, =pxCurrentTCB  \n"
        "  ldr r1, [r1]           \n"
        "  ldr r0, [r1]           \n"  /* r0 = task->sp(伪造帧底) */
        "  add r0, r0, #96        \n"  /* 帧顶(64B)+ svc 压帧(32B),避免覆盖伪造帧 */
        "  msr psp, r0            \n"
        "  isb                    \n"
        "  svc 0                  \n"  /* 启动第一个任务 */
    );
}

/* 阻塞延时:睡眠计数个 tick(1 tick = 1ms),期间让出 CPU */
void rtos_delay(uint32 ms)
{
    if (ms == 0)
        return;

    uint32 mask = rtos_enter_critical();  /* 防止 SysTick 插队把我们挂回就绪队列 */

    pxCurrentTCB->sleep = ms;             /* 睡眠计数 */
    queue_add(sleepqueue, pxCurrentTCB);  /* 挂入睡眠队列 */

    SCB->ICSR |= SCB_ICSR_PENDSVSET;      /* 请求切换,把自己切下去 */
    rtos_exit_critical(mask);             /* 恢复 → 中断开 → PendSV 执行,切到下一个任务 */
    /* 睡眠到期被唤醒、重新拿到 CPU 后,从这里继续返回 */
}














