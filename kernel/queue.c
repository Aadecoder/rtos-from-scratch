#include "../Inc/queue.h"
#include "../Inc/port.h"
#include "../Inc/scheduler.h"
#include "../Inc/tasks.h"

/* Unblocks the Receiver */
static void unblockFirstWaitingTask(tcb_t **list){
    tcb_t *task = *list;
    if (task == NULL){
        return;
    }
    *list = task->wt_next;
    task->wt_next = NULL;
    task->wait_obj = NULL;

    if (task->state == BLOCKED){
        removeTaskFromDelayQueue(task);
    }

    task->state = READY;
    addTaskToReadyQueue(task);
}

/* Blocks the sender */
static void blockCurrentTaskOnQueue(tcb_t **wait_list, queue_t *queue, uint32_t timeout){
    tcb_t *task = pCurrentTcb;
    
    task->wt_next = *wait_list;
    *wait_list = task;
    task->wait_obj = (void *)queue;

    removeTaskFromReadyQueue(task);
    task->state = BLOCKED;

    if (timeout != portMAX_DELAY){
        task->delay_ticks = timeout;
        addTaskToDelayQueue(task);
    }
}

/* Creates and Initializes a queue */
queueHandle_t queueCreate(uint32_t length, uint32_t itemSize){
    queue_t *queue;
    uint32_t mask = enterCritical();

    queue = (queue_t *)pPortMalloc(sizeof(queue_t));
    if (queue->buffer == NULL){
        exitCritical(mask);
        return NULL; 
    }

    queue->length = length;
    queue->itemSize = itemSize;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->tx_wait_list = NULL;
    queue->rx_wait_list = NULL;

    exitCritical(mask);
    return queue;
}

/* Sends stuff to queue */
uint32_t queueSend(queueHandle_t queue, const void *item, uint32_t timeout){
    uint32_t mask;
    uint32_t result = 1;
    uint8_t *buf = (uint8_t *)queue->buffer;

    mask = enterCritical();
    
    while (queue->count == queue->length){
        if (timeout == 0){
            result = 0;
            goto exit;
        }
        blockCurrentTaskOnQueue(&queue->tx_wait_list, queue, timeout);
        exitCritical(mask);
        taskYield();
        mask = enterCritical();

        if (queue->count < queue->length){
            break;
        }
        result = 0;
        goto exit;
    }

    for(uint32_t i = 0; i < queue->itemSize; i++){
        buf[queue->tail * queue->itemSize + i] = ((const uint8_t *)item)[i];
    }

    queue->tail++;
    if (queue->tail >= queue->length){
        queue->tail = 0;
    }
    queue->count++;
    unblockFirstWaitingTask(&queue->rx_wait_list);

exit:
    exitCritical(mask);
    return result;
}

/* Receives stuff from queue */
uint32_t queueReceive(queueHandle_t queue, void *buffer, uint32_t timeout){
    uint32_t mask;
    uint32_t result = 1;
    uint8_t *buf = (uint8_t *)queue->buffer;

    mask = enterCritical();

    while(queue->count == 0){
        if (timeout == 0){
            result = 0;
            goto exit;
        }

        blockCurrentTaskOnQueue(&queue->rx_wait_list, queue, timeout);
        exitCritical(mask);
        taskYield();
        mask = enterCritical();

        if (queue->count > 0){
            break;
        }
        result = 0;
        goto exit;
    }

    for (uint32_t i = 0; i < queue->itemSize; i++){
        ((uint8_t *)buffer)[i] = buf[queue->head * queue->itemSize + i];
    }

    queue->head++;
    if (queue->head >= queue->length){
        queue->head = 0;
    }
    queue->count--;

    unblockFirstWaitingTask(&queue->tx_wait_list);

exit:
    exitCritical(mask);
    return result;
}
