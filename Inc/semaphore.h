#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "stm32f4xx.h"
#include "tasks.h"

typedef struct{
    uint32_t count;
    uint32_t maxCount;
    tcb_t *waitList;
}semaphore_t;

typedef semaphore_t *semaphoreHandle_t;

semaphoreHandle_t semaphoreCreateBinary(void);
semaphoreHandle_t semaphoreCreateCounting(uint32_t maxCount, uint32_t initialCount);
uint32_t semaphoreTake(semaphoreHandle_t sem, uint32_t timeout);
uint32_t semaphoreGive(semaphoreHandle_t sem);

#endif
