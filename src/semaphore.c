#include "semaphore.h"

semaphore* semaphore_create(uint32 nums)
{
    heapnode* h = heap32pool_alloc();
    if (h == 0)
        return 0;
    semaphore* sph = (semaphore*)h->heapptr;
    sph->sphlist = create_queue();
    if (sph->sphlist == 0)
    {
        heap32pool_free(h);
        return 0;
    }
    sph->nums = nums;
    sph->heaphand = h;
    return sph;
}

void semaphore_p(semaphore* sph)
{
    uint32 mask = rtos_enter_critical();

    if (sph->nums == 0)
    {
        queue_add(sph->sphlist, pxCurrentTCB);
        SCB->ICSR |= SCB_ICSR_PENDSVSET;   /* 请求切换,把自己切下去 */
        rtos_exit_critical(mask);                           /* 恢复(通常为 0)→ 中断开 → PendSV 执行 */
        /* 注意:不要在临界区里调用 P(阻塞需要开中断让 PendSV 跑,会死锁) */
        /* 被 V 唤醒、重新拿到 CPU 后从这里继续返回 */
    }
    else
    {
        sph->nums -= 1;
        rtos_exit_critical(mask);
    }
}

void semaphore_v(semaphore* sph)
{
    uint32 mask = rtos_enter_critical();

    if (sph->nums == 0 && sph->sphlist->head != 0)   /* 有任务在等 */
    {
        tcb* q = queue_pop_head(sph->sphlist);       /* 唤醒队头 */
        queue_add(readyqueue[q->priority], q);
        if (q->priority < pxCurrentTCB->priority)    /* 被唤醒的更高 → 立即切换 */
        {
            queue_add(readyqueue[pxCurrentTCB->priority], pxCurrentTCB); /* 先把自己挂回就绪队列 */
            SCB->ICSR |= SCB_ICSR_PENDSVSET;
        }
        rtos_exit_critical(mask);
    }
    else
    {
        sph->nums += 1;                              /* 没任务等 → 发一个令牌 */
        rtos_exit_critical(mask);
    }
}

void semaphore_destroy(semaphore* sph)
{
    queue_destroy(sph->sphlist);
    heap32pool_free(sph->heaphand);
}


semaphore* semaphore_tcb;
semaphore* semaphore_stack; 
semaphore semaphore_heap32;
semaphore* semaphore_heap64;
semaphore* semaphore_heap128;
semaphore* semaphore_heap256;

void os_semaphore_init(void)
{
    semaphore_heap32.nums = 64;
    semaphore_heap32.sphlist = create_queue();
    semaphore_tcb = semaphore_create(7);
    semaphore_stack = semaphore_create(7);
    semaphore_heap64 = semaphore_create(32);
    semaphore_heap128 = semaphore_create(16);
    semaphore_heap256 = semaphore_create(24);
}
