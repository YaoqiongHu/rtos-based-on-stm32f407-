#include "int.h"
#include "queue.h"
#include "memory.h"

/* 队列函数内部自带临界区,但用"保存/恢复 PRIMASK"实现可嵌套:
   从 semaphore_p/rtos_delay/PendSV 等外层临界区里调用时,不会把外层误开 */

queue* create_queue(void)
{
    /* 新队列对象来自 heap32pool_alloc(其内部有 P + 临界区),
       队列字段只初始化自己独占的新内存,无需再套临界区 */
    heapnode* h = heap32pool_alloc();
    if (h == 0)
        return 0;
    queue* queuelist = (queue*)h->heapptr;
    queuelist->heap_hand = h;
    queuelist->head = 0;
    queuelist->tail = 0;
    return queuelist;
}

uint8 queue_add(queue* queuelist,tcb* task)
{
    uint32 mask = rtos_enter_critical();

    if (task->queuenode_hand == 0)              /* 任务没有节点 */
    {
        rtos_exit_critical(mask);
        return 0;
    }
    if (task->queuenode_hand->queueowner != 0)  /* 已在某个队列里:防重复入队 */
    {
        rtos_exit_critical(mask);
        return 0;
    }
    queuenode* task_queuenode = task->queuenode_hand;
    task_queuenode->queueowner = queuelist;
    if(queuelist->head == 0 && queuelist->tail == 0)//如果队列为空
    {
        task_queuenode->next = 0;
        task_queuenode->last = 0;
        queuelist->head = task_queuenode;
        queuelist->tail = task_queuenode;
    }
    else//队列不为空
    {
        task_queuenode->next = 0;
        task_queuenode->last = queuelist->tail;
        queuelist->tail->next = task_queuenode; 
        queuelist->tail = task_queuenode;
    }
    rtos_exit_critical(mask);
    return 1;
}



void queue_remove_node(tcb* task)
{
    uint32 mask = rtos_enter_critical();

    if (task->queuenode_hand == 0 || task->queuenode_hand->queueowner == 0)
    {
        rtos_exit_critical(mask);
        return;
    }
    if(task->queuenode_hand->last == 0)//被删除的任务在队首
    {   
        if(task->queuenode_hand->next != 0)//被删除的任务后继有任务
            {
                task->queuenode_hand->next->last = 0;
                task->queuenode_hand->queueowner->head = task->queuenode_hand->next;
            } 
        else//被删除的任务后继没有任务
            task->queuenode_hand->queueowner->head = task->queuenode_hand->queueowner->tail = 0;
    }
    else//被删除的任务不在队首 
    {   
        if(task->queuenode_hand->next != 0)//被删除的任务也不在队尾
        {    
            task->queuenode_hand->next->last = task->queuenode_hand->last;
            task->queuenode_hand->last->next = task->queuenode_hand->next;
        }
        else//被删除的任务在队尾 
        {
            task->queuenode_hand->last->next = 0;
            task->queuenode_hand->queueowner->tail = task->queuenode_hand->last;
        }
    } 
    task->queuenode_hand->queueowner = 0;
    task->queuenode_hand->last = 0;
    task->queuenode_hand->next = 0;
    rtos_exit_critical(mask);
}


tcb* queue_pop_head(queue* queuelist)
{
    uint32 mask = rtos_enter_critical();

    if(queuelist->head == 0)
    {
        rtos_exit_critical(mask);
        return 0;
    }
    queuenode* head = queuelist->head;
    tcb* task = head->tcbowner;

    queuelist->head = head->next;
    if (queuelist->head)
        queuelist->head->last = 0;     
    else
        queuelist->tail = 0; 
    task->queuenode_hand->queueowner = 0;
    task->queuenode_hand->next = 0;
    task->queuenode_hand->last = 0;
    rtos_exit_critical(mask);
    return task;
}

void queue_destroy(queue* queuelist)
{
    uint32 mask = rtos_enter_critical();

    while(queuelist->head != 0)
    {
        queue_remove_node(queuelist->head->tcbowner);
    }
    heap32pool_free(queuelist->heap_hand);
    rtos_exit_critical(mask);
}

uint8 queue_is_empty(queue* q)
{
   return q->head == 0; 
}
