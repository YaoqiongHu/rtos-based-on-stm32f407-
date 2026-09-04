#include "memory.h"

#define TASK_MAXNUMS  (7)

#define TCBPOOL_ADDR  (0x2001C000)

#define STACK_ADDR  (0x2001C000)
#define STACK_SIZE  (0x2000)

#define HEAP256_ADDR   ((void*)0x2001C800)
#define HEAP256_SIZE   (256)
#define HEAP256_MAXNUMS    (24)

#define HEAP128_ADDR   ((void*)0x2001E000)
#define HEAP128_SIZE   (128)
#define HEAP128_MAXNUMS    (16)

#define HEAP64_ADDR   ((void*)0x2001E800)
#define HEAP64_SIZE   (64)
#define HEAP64_MAXNUMS    (32)

#define HEAP32_ADDR   ((void*)0x2001F000)
#define HEAP32_SIZE   (32)
#define HEAP32_MAXNUMS    (64)


 
static tcbnode tcbpool[TASK_MAXNUMS];
static tcbnode* tcbpoolhead = (tcbnode*)tcbpool;

static stacknode stackpool[TASK_MAXNUMS];
static stacknode* stackpoolhead = (stacknode*)stackpool;

static heapnode heap256pool[HEAP256_MAXNUMS];//256B内存块池
static heapnode* heap256poolhead = (heapnode*)heap256pool;
static heapnode heap128pool[HEAP128_MAXNUMS];
static heapnode* heap128poolhead = (heapnode*)heap128pool;
static heapnode heap64pool[HEAP64_MAXNUMS];
static heapnode* heap64poolhead = (heapnode*)heap64pool;
static heapnode heap32pool[HEAP32_MAXNUMS];
static heapnode* heap32poolhead = (heapnode*)heap32pool;

void tcbpool_init(void)
{   
    tcbpoolhead = (tcbnode*)tcbpool;  
    for(uint8 i = 0; i < TASK_MAXNUMS-1; i++)
    {
        tcbpool[i].tcbptr = (void*)(TCBPOOL_ADDR + i*sizeof(tcb));
        tcbpool[i].next = &tcbpool[i+1];
    }
    tcbpool[TASK_MAXNUMS-1].tcbptr = (void*)(TCBPOOL_ADDR + (TASK_MAXNUMS - 1)*sizeof(tcb));
    tcbpool[TASK_MAXNUMS-1].next = 0;
}

tcbnode* tcb_alloc(void)
{
    semaphore_p(semaphore_tcb);
    uint32 mask = rtos_enter_critical();
    tcbnode* tcbspace = tcbpoolhead;
    tcbpoolhead = tcbpoolhead->next;
    tcbspace->next = 0;
    rtos_exit_critical(mask);
    return tcbspace;
}
void tcb_free(tcbnode* tcbspace)
{   
    uint32 mask = rtos_enter_critical();
    tcbspace->next = tcbpoolhead;
    tcbpoolhead = tcbspace;
    rtos_exit_critical(mask);
    semaphore_v(semaphore_tcb);
}


void stackpool_init(void)
{    
    stackpoolhead = (stacknode*)stackpool;  
    for(uint8 i = 0; i < TASK_MAXNUMS-1; i++)
    {
        
        stackpool[i].stackptr = (void*)(STACK_ADDR - i*STACK_SIZE);
        stackpool[i].next = &stackpool[i+1];
    }
    stackpool[TASK_MAXNUMS - 1].stackptr = (void*)(STACK_ADDR - (TASK_MAXNUMS - 1)*STACK_SIZE);
    stackpool[TASK_MAXNUMS - 1].next = 0;
}
stacknode* stack_alloc(void)
{   
    semaphore_p(semaphore_stack);
    uint32 mask = rtos_enter_critical();
    stacknode* stackspace = stackpoolhead;
    stackpoolhead = stackpoolhead->next;
    stackspace->next = 0;
    rtos_exit_critical(mask);
    return stackspace;
}
void stack_free(stacknode* stackspace)
{
    uint32 mask = rtos_enter_critical();
    stackspace->next = stackpoolhead;
    stackpoolhead = stackspace;
    rtos_exit_critical(mask);
    semaphore_v(semaphore_stack);
}

void heappool_init(void* heap_addr,uint32 heap_size,uint32 heap_nums,heapnode* heappool,heapnode* (*heappoolhead))
{   
    *heappoolhead = heappool;
    for(uint8 i = 0; i < heap_nums-1; i++)
    {   
        heappool[i].heapptr = (void*)(heap_addr + i * heap_size);   
        heappool[i].next = &heappool[i+1];
    }
    heappool[heap_nums-1].heapptr = (void*)(heap_addr) + (heap_nums -1) * heap_size;
    heappool[heap_nums-1].next = 0;
}

heapnode* heap_alloc(heapnode* (*heappoolhead))
{
    uint32 mask = rtos_enter_critical();
    heapnode* heapspace = *heappoolhead;
    if (heapspace == 0)
    {
        rtos_exit_critical(mask);
        return 0;
    }
    *heappoolhead = (*heappoolhead)->next;   /* 先取链上的下一个(和 tcb/stack 池一致) */
    heapspace->next = 0;    
    rtos_exit_critical(mask);
    return heapspace;
}

void heap_free(heapnode* heapspace,heapnode* (*heappoolhead))
{   
    uint32 mask = rtos_enter_critical();
    heapspace->next = *heappoolhead;
    *heappoolhead = heapspace;
    rtos_exit_critical(mask);
}


void heap256pool_init(void) 
{   
    heappool_init(HEAP256_ADDR,HEAP256_SIZE,HEAP256_MAXNUMS,(heapnode*) heap256pool,&heap256poolhead);
}
void heap128pool_init(void)
{
    heappool_init(HEAP128_ADDR,HEAP128_SIZE,HEAP128_MAXNUMS,(heapnode*) heap128pool,&heap128poolhead);
}
void heap64pool_init(void)
{
    heappool_init(HEAP64_ADDR,HEAP64_SIZE,HEAP64_MAXNUMS,(heapnode*) heap64pool,&heap64poolhead);
}
void heap32pool_init(void)
{
    heappool_init(HEAP32_ADDR,HEAP32_SIZE,HEAP32_MAXNUMS,(heapnode*) heap32pool,&heap32poolhead);
}
        
heapnode* heap256pool_alloc(void)
{   
    semaphore_p(semaphore_heap256);
     return heap_alloc(&heap256poolhead);
}
heapnode* heap128pool_alloc(void)
{ 
    semaphore_p(semaphore_heap128);
    return heap_alloc(&heap128poolhead);
}
heapnode* heap64pool_alloc(void)
{ 
    semaphore_p(semaphore_heap64);
    return heap_alloc(&heap64poolhead);
}
heapnode* heap32pool_alloc(void)
{ 
    semaphore_p(&semaphore_heap32);
    return heap_alloc(&heap32poolhead);
}

void heap256pool_free(heapnode* heap256space)
{
     heap_free(heap256space,&heap256poolhead);
     semaphore_v(semaphore_heap256);
}
void heap128pool_free(heapnode* heap128space)
{
     heap_free(heap128space,&heap128poolhead);
     semaphore_v(semaphore_heap128);
}
void heap64pool_free(heapnode* heap64space)
{
     heap_free(heap64space,&heap64poolhead);
     semaphore_v(semaphore_heap64);
}
void heap32pool_free(heapnode* heap32space)
{
     heap_free(heap32space,&heap32poolhead);
     semaphore_v(&semaphore_heap32);
}


