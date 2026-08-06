#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "../Inc/types.h"
#include "../Inc/config.h"

typedef struct{
    tcb_t* head;
    tcb_t* tail;
}tcbQueue_t;

extern tcbQueue_t readyQueue[NO_OF_THREADS];
extern tcbQueue_t blockedQueue[NO_OF_THREADS];
extern tcbQueue_t delayedQueue[NO_OF_THREADS];

// Select Next Task for Round Robin Scheduling
void selectNextTaskRoundRobin(tcb_t* taskHandler);

// Select Next Task for Preemptive Scheduling
void selectNextTask(void);

// Yield current task
void taskYield(void);

// Helper function used to find the highest priority of tasks
uint32_t findHighestPriority(void);


#endif
