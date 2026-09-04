#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include "rtos.h"
#include "core.h"



typedef struct Semaphore
{
    queue* sphlist;
    uint32 nums;
    heapnode* heaphand;
}semaphore;

semaphore* semaphore_create(uint32 nums);
void semaphore_p(semaphore* sph);
void semaphore_v(semaphore* sph);
void semaphore_destroy(semaphore* sph);



#endif