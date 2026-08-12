#ifndef QUEUE_H
#define QUEUE_H

#include "stm32f4xx.h"
#include "tasks.h"

typedef struct{
    void *buffer;
    uint32_t itemSize;
    uint32_t length;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    tcb_t *tx_wait_list;
    tcb_t *rx_wait_list;
}queue_t;

typedef queue_t *queueHandle_t;

queueHandle_t queueCreate(uint32_t length, uint32_t itemSize);
uint32_t queueSend(queueHandle_t queue, const void *item, uint32_t timeout);
uint32_t queueReceive(queueHandle_t queue, void *buffer, uint32_t timeout);

#endif

