#include "../Inc/semaphore.h"
#include "../Inc/port.h"
#include "../Inc/scheduler.h"

static void prevUnblockFirstWaiter(semaphore_t *sem){
    tcb_t *task = sem->waitList;
    if (task == NULL){
        return;
    }
    sem->waitList = task->wt_next;
    task->wt_next = NULL;
    task->wait_obj = NULL;

    if (task->state == BLOCKED){
        removeTaskFromDelayQueue(task);
    }
    task->state = READY;
    addTaskToReadyQueue(task);
}

semaphoreHandle_t semaphoreCreateBinary(void){
    semaphore_t *sem;
    uint32_t mask = enterCritical();
    sem = (semaphore_t *)pPortMalloc(sizeof(semaphore_t));
    if (sem){
        sem->count = 0;
        sem->maxCount = 1;
        sem->waitList = NULL;
    }
    exitCritical(mask);
    return sem;
}

semaphoreHandle_t semaphoreCreateCounting(uint32_t maxCount, uint32_t initialCount){
    semaphore_t *sem;
    uint32_t mask = enterCritical();

    sem = (semaphore_t *)pPortMalloc(sizeof(semaphore_t));

    if(sem){
        sem->count = initialCount;
        sem->maxCount = maxCount;
        sem->waitList = NULL;
    }

    exitCritical(mask);
    return sem;
}

uint32_t semaphoreTake(semaphoreHandle_t sem, uint32_t timeout){
    uint32_t mask = enterCritical();

    while(sem->count == 0){
        if (timeout == 0){
            exitCritical(mask);
            return 0;
        }

        tcb_t *task = pCurrentTcb;
        task->wt_next = sem->waitList;
        sem->waitList = task;
        task->wait_obj = (void*) sem;

        removeTaskFromReadyQueue(task);
        task->state = BLOCKED;
        if (timeout != portMAX_DELAY){
            task->delay_ticks = timeout;
            addTaskToDelayQueue(task);
        }

        exitCritical(mask);
        taskYield();
        mask = enterCritical();

        if (task->state == READY){
            break;
        }

        exitCritical(mask);
        return 0;
    }
    sem->count--;
    exitCritical(mask);
    return 1;
}

uint32_t semaphoreGive(semaphoreHandle_t sem){
    uint32_t mask = enterCritical();

    if(sem->count >= sem->maxCount){
        exitCritical(mask);
        return 0;
    }

    sem->count++;
    prevUnblockFirstWaiter(sem);

    if(readyBitMap > (1 << pCurrentTcb->priority)){
        portNVIC_INT_CTRL = portNVIC_PENDSVSET_BIT;
    }

    exitCritical(mask);
    return 1;
}
