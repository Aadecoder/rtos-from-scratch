#include "../Inc/scheduler.h"
#include "../Inc/types.h"
#include "../Inc/config.h"
#include "../Inc/tasks.h"
#include "../Inc/port.h"


uint32_t readyBitMap = 0;

void schedulerInit(tcb_t* tcbQueue){
	for (uint32_t i=0; i<NO_OF_THREADS; i++){
		readyQueue[i].head = NULL;
		readyQueue[i].tail = NULL;
	}
    readyBitMap = 0;
}

uint32_t findHighestPriority(void){
    if (readyBitMap == 0){
        return 0;
    }
    return 31 - __builtin_clz(readyBitMap);
}

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

    if (task->readyNxt){
        list->head = task->readyNxt;
        list->head->readyPrev = NULL;

        list->tail->readyNxt = task;
        task->readyPrev = list->tail;
        task->readyNxt = NULL;
        list->tail = task;
    }

    task->state = RUNNING;
    pCurrentTcb = task;
}

void taskYield(void){
    uint32_t mask = enterCritical();
    portNVIC_INT_CTRL = portNVIC_PENDSVSET_BIT;
    exitCritical(mask);
}







