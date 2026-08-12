#ifndef PORT_H
#define PORT_H
#include "types.h"
#include "stm32f4xx.h"

#define PRIORITY_MASK   0x0FUL
#define MAX_PRIORITIES  16

#define NO_CRITICAL_SECTION     0

void setupTimerInterrupt(void);
void startFirstTask(void);
void yield(void);
void pendSV(void);

uint32_t enterCritical(void);
void exitCritical(uint32_t mask);

void *pPortMalloc(uint32_t size);

#endif
