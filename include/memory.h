#ifndef MEMORY_H
#define MEMORY_H
#include "int.h"

typedef struct Semaphore semaphore;  

extern semaphore* semaphore_tcb;
extern semaphore* semaphore_stack;
extern semaphore semaphore_heap32;
extern semaphore* semaphore_heap64;
extern semaphore* semaphore_heap128;
extern semaphore* semaphore_heap256;

typedef struct TCBnode
{
    void* tcbptr;
    struct TCBnode* next;  

}tcbnode;

typedef struct Stacknode
{
    void* stackptr;
    struct Stacknode* next;  

}stacknode;

typedef struct Heapnode
{
    void* heapptr;
    struct Heapnode* next;
}heapnode;

typedef struct Queuenode queuenode;
typedef struct TCB 
{
    
    uint32 sp;
    uint32 pid;
    uint32 priority; 
    uint32 sleep;   
    stacknode* stack_hand;
    queuenode* queuenode_hand;
    tcbnode* tcb_hand;

}tcb;
//tcb块管理
void tcbpool_init(void);
tcbnode* tcb_alloc(void);
void tcb_free(tcbnode* a);

//stack块管理
void stackpool_init(void);
stacknode* stack_alloc(void);
void stack_free(stacknode* heapspace);

//heap快管理
void heap256pool_init(void);
void heap128pool_init(void);
void heap64pool_init(void);
void heap32pool_init(void);

heapnode* heap256pool_alloc(void);
heapnode* heap128pool_alloc(void);
heapnode* heap64pool_alloc(void);
heapnode* heap32pool_alloc(void);

void heap256pool_free(heapnode* heap256space);
void heap128pool_free(heapnode* heap128space);
void heap64pool_free(heapnode* heap64space);
void heap32pool_free(heapnode* heap32space);


void semaphore_p(semaphore* sph);
void semaphore_v(semaphore* sph);

/* 可嵌套临界区(保存/恢复 PRIMASK)。memory.h 是公共底层,声明放这里 */
uint32 rtos_enter_critical(void);
void   rtos_exit_critical(uint32 mask);

#endif