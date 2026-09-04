#ifndef QUEUE_H
#define QUEUE_H
#include "memory.h"
typedef struct Queue queue;    /* 前向声明 */

typedef struct Queuenode {
    struct Queuenode* last;
    struct Queuenode* next;
    tcb*  tcbowner;        /* 属于哪个任务 */
    queue*  queueowner;      /* 在哪个队列里 */
    heapnode*  heap_hand;       /* 自己占用的堆池节点(还内存用) */
} queuenode;

struct Queue {
    queuenode* head;
    queuenode* tail;
    heapnode* heap_hand;
};

queue* create_queue(void);
uint8 queue_add(queue* q, tcb* task);
tcb* queue_pop_head(queue* queuelist);
void queue_remove_node(tcb* task);
void queue_destroy(queue* queuelist);
uint8 queue_is_empty(queue* q);
#endif