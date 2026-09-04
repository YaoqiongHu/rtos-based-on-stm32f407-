#include "core.h"
#include "queue.h"

/* rtos.c 里定义的全局,port 层要访问 */
extern tcb*   pxCurrentTCB;
extern queue* readyqueue[4];
extern queue* sleepqueue;
void vTaskSwitchContext(void);

/* ========== 中断配置 + SysTick 初始化:1ms 一个 tick ========== */
void systick_init(void)
{
    /* PendSV、SysTick 都必须是最低优先级(0xFF = 数值 15) */
    /* PendSV 最低:切换永远等其它中断处理完;SysTick 与它同级,不会互相打断 */
    SCB->SHPR3 = (0xFFUL << 16) | (0xFFUL << 24);

    SysTick->RVR = 168000000UL / 1000 - 1;   /* 168MHz,1ms 溢出一次 */
    SysTick->CVR = 0;
    SysTick->CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
}

/* ========== SysTick 中断:睡眠唤醒 + 时间片 + 预约切换 ========== */
void SysTick_Handler(void)
{
    /* 1. 遍历睡眠队列:计数减 1,减到 0 的任务重新就绪 */
    queuenode* n = sleepqueue->head;
    while (n != 0)
    {
        queuenode* next = n->next;            /* 先存后继(摘除会释放节点) */
        tcb* task = n->tcbowner;

        if (task->sleep > 0)
            task->sleep--;
        if (task->sleep == 0)
        {
            queue_remove_node(task);                       /* 从睡眠队列摘除 */
            queue_add(readyqueue[task->priority], task);   /* 重新就绪 */
        }
        n = next;
    }

    /* 2. 时间片到期:当前任务挂回自己优先级的就绪队列队尾 */
    /*    (重复入队防护在 queue_add 里:已在队列则直接返回,正好兜住首启竞态) */
    if (pxCurrentTCB != 0)
        queue_add(readyqueue[pxCurrentTCB->priority], pxCurrentTCB);

    /* 3. 预约切换:真正切换等所有中断返回后由 PendSV 执行 */
    SCB->ICSR |= SCB_ICSR_PENDSVSET;
}

/* ========== SVC:一次性启动第一个任务(只加载,不保存)========== */
/* 注意:rtos_start 在 svc 0 之前已设置 SPSEL=1 + PSP,所以进入本 handler 时
   LR 里的 EXC_RETURN 就是 0xFFFFFFF9(返回线程模式 + 用 PSP),bx lr 直接从任务帧弹栈。 */
__attribute__((naked)) void SVC_Handler(void)
{
    __asm volatile(
        "  ldr r3, =pxCurrentTCB  \n"  /* 第一个任务 */
        "  ldr r1, [r3]           \n"
        "  ldr r0, [r1]           \n"  /* r0 = task->sp(伪造帧最低位,指向 r4) */
        "  ldmia r0!, {r4-r11}    \n"  /* 弹出 r4-r11 */
        "  msr psp, r0            \n"  /* PSP 指向硬件帧的 r0 槽 */
        "  isb                    \n"
        "  mov r0, #0             \n"
        "  msr basepri, r0        \n"  /* 打开中断(任务跑起来后 SysTick 才能工作) */
        "  bx lr                  \n"  /* EXC_RETURN=0xFFFFFFF9 → 从 PSP 弹 8 寄存器 → 任务开跑 */
    );
}

/* ========== PendSV:日常上下文切换(保存当前 + 加载下一个)========== */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile(
        "  mrs r0, psp            \n"  /* 当前任务栈顶 */
        "  ldr r3, =pxCurrentTCB  \n"
        "  ldr r2, [r3]           \n"  /* 当前任务 TCB */
        "  stmdb r0!, {r4-r11}    \n"  /* 保存 r4-r11(硬件已存 r0-r3/r12/lr/pc/xpsr) */
        "  str r0, [r2]           \n"  /* 新栈顶存回 TCB->sp */
        "  stmdb sp!, {r3, lr}    \n"  /* 保护 r3、返回地址(bl 会破坏) */
        "  cpsid i                \n"  /* 选任务期间关中断,防高优先级 ISR 干扰 */
        "  bl vTaskSwitchContext  \n"  /* 弹出下一个任务 */
        "  cpsie i                \n"
        "  ldmia sp!, {r3, lr}    \n"
        "  ldr r1, [r3]           \n"  /* 新任务 TCB */
        "  ldr r0, [r1]           \n"  /* 新任务栈顶 */
        "  ldmia r0!, {r4-r11}    \n"  /* 恢复 r4-r11 */
        "  msr psp, r0            \n"  /* 切换 PSP */
        "  isb                    \n"
        "  bx lr                  \n"  /* 异常返回,硬件弹剩余 8 寄存器 */
    );
}
