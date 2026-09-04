#ifndef MUTEX_H
#define MUTEX_H
#include "queue.h"
#include "rtos.h"
#include "core.h"

typedef struct Mutex
{
    tcb* tcbowner;      /* 当前持有者,0 = 空闲 */
    queue* mutexlist;   /* 等待队列(拿不到锁而阻塞的任务) */
    uint32 priority;    /* 持有者的原始优先级(继承后恢复用) */
    heapnode* heaphand; /* 自身内存块句柄 */
} mutex;

mutex* mutex_create(void);
void mutex_take(mutex* m);
void mutex_give(mutex* m);
void mutex_destroy(mutex* m);

#endif
