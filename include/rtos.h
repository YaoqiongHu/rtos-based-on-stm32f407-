#ifndef RTOS_H
#define RTOS_H

#include "queue.h"

#define MAX_PRIO  4

extern tcb*   pxCurrentTCB;          /* 当前运行任务 */
extern queue* readyqueue[MAX_PRIO];  /* 每优先级一条就绪队列 */
extern queue* sleepqueue;            /* 睡眠队列 */

void rtos_init(void);
tcb* rtos_create_task(void (*entry)(void), uint32 priority, uint32 pid);
void rtos_start(void);
void vTaskSwitchContext(void);
void rtos_delay(uint32 ms);
void os_semaphore_init(void);
void systick_init(void);            /* 在 port.c 里实现 */

semaphore* semaphore_create(uint32 nums);
void semaphore_p(semaphore* sph);
void semaphore_v(semaphore* sph);
void semaphore_destroy(semaphore* sph);

#endif
