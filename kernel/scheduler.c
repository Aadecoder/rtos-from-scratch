#include "../Inc/scheduler.h"
#include "../Inc/types.h"
#include "../Inc/config.h"
#include "../Inc/tasks.h"
#include "../Inc/port.h"


uint32_t readyBitMap = 0;

/* Initializes the RTOS Scheduler */
void schedulerInit(tcb_t* tcbQueue){
	for (uint32_t i=0; i<NO_OF_THREADS; i++){
		readyQueue[i].head = NULL;
		readyQueue[i].tail = NULL;
	}
    readyBitMap = 0;
}

/* Finds the Highest Priority out of the Ready Queue */
uint32_t findHighestPriority(void){
    if (readyBitMap == 0){
        return 0;
    }
    return 31 - __builtin_clz(readyBitMap);
}

/* Finds the next highest priority task to allowed to run */
void selectNextTask(void){
	tcb_t *task;
	uint32_t priority;
    tcbQueue_t  *list;

	if (readyBitMap == 0){
		return;
	}

    priority = findHighestPriority();
    list = &readyQueue[priority];
    task = list->head;

    if (task == NULL){
        readyBitMap &= ~(1 << priority);
        return;
    }

    if (task->rdy_next){
        list->head = task->rdy_next;
        list->head->rdy_prev = NULL;

        list->tail->rdy_next = task;
        task->rdy_prev = list->tail;
        task->rdy_next = NULL;
        list->tail = task;
    }

    task->state = RUNNING;
    pCurrentTcb = task;
}

/* Yields the task */
void taskYield(void){
    uint32_t mask = enterCritical();
    portNVIC_INT_CTRL = portNVIC_PENDSVSET_BIT;
    exitCritical(mask);
}
