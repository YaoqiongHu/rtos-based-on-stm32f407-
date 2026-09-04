#include "mutex.h"

mutex* mutex_create(void)
{
    heapnode* h = heap32pool_alloc();
    if (h == 0)
        return 0;
    mutex* m = (mutex*)(h->heapptr);
    m->mutexlist = create_queue();
    if (m->mutexlist == 0)
    {
        heap32pool_free(h);
        return 0;
    }
    m->tcbowner = 0;
    m->priority = 0;
    m->heaphand = h;
    return m;
}

/* 拿锁。锁被占用时:若等锁的任务优先级比持有者高,先把持有者提升到
   自己的优先级(优先级继承,让持有者尽快跑完释放),然后阻塞等待 */
void mutex_take(mutex* m)
{
    uint32 mask = rtos_enter_critical();

    if (m->tcbowner == 0)                 /* 空闲:直接拿走 */
    {
        m->tcbowner = pxCurrentTCB;
        m->priority = pxCurrentTCB->priority;   /* 记住原始优先级 */
        rtos_exit_critical(mask);
        return;
    }

    if (m->tcbowner == pxCurrentTCB)      /* 自己已持有:防自锁死锁 */
    {
        rtos_exit_critical(mask);
        return;
    }

    /* 被占用 → 无条件阻塞(否则互斥被破坏) */
    if (pxCurrentTCB->priority < m->tcbowner->priority)   /* 等锁者更高 → 继承 */
    {
        tcb* L = m->tcbowner;
        if (L->queuenode_hand->queueowner != 0 &&
            L->queuenode_hand->queueowner != sleepqueue)  /* L 在就绪队列:挪到新优先级 */
        {
            queue_remove_node(L);
            L->priority = pxCurrentTCB->priority;
            queue_add(readyqueue[L->priority], L);
        }
        else                                /* L 在睡眠/在运行:只改字段,时机到了自然生效 */
        {
            L->priority = pxCurrentTCB->priority;
        }
    }

    queue_add(m->mutexlist, pxCurrentTCB); /* 阻塞等待 */
    SCB->ICSR |= SCB_ICSR_PENDSVSET;
    rtos_exit_critical(mask);              /* 恢复 → 中断开 → PendSV 切走 */
}

/* 释放锁:恢复持有者原始优先级,把锁转交给队头等待者(它从 take 恢复后
   不需要再抢锁 —— 若这里把 owner 清空,等待者以为拿到锁但 owner=0,
   第三方会乘虚而入,互斥被破坏) */
void mutex_give(mutex* m)
{
    uint32 mask = rtos_enter_critical();

    if (m->tcbowner != pxCurrentTCB)      /* 非持有者释放:忽略 */
    {
        rtos_exit_critical(mask);
        return;
    }

    pxCurrentTCB->priority = m->priority; /* 释放者恢复被继承抬高的优先级 */

    if (m->mutexlist->head != 0)          /* 有等待者:锁直接转交队头 */
    {
        tcb* waketask = queue_pop_head(m->mutexlist);
        m->tcbowner = waketask;           /* 转交,不清空 */
        m->priority = waketask->priority; /* 新持有者的原始优先级 */
        queue_add(readyqueue[waketask->priority], waketask);
        if(waketask->priority < pxCurrentTCB->priority)
        {
            queue_add(readyqueue[pxCurrentTCB->priority],pxCurrentTCB);
            SCB->ICSR |= SCB_ICSR_PENDSVSET;
        }
    }
    else                                  /* 没人等才清空 */
    {
        m->tcbowner = 0;
    }
    rtos_exit_critical(mask);
}

void mutex_destroy(mutex* m)
{
    queue_destroy(m->mutexlist);          /* 注意:还有任务在等时,它们会永久阻塞 */
    heap32pool_free(m->heaphand);
}
