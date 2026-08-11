#ifndef TASKS_H
#define TASKS_H
#include "types.h"
#include "config.h"
#include "stm32f4xx.h"

extern tcb_t* pCurrentTcb;
extern tcb_t taskArray[MAX_TASKS];
extern uint32_t taskCount;

// Create a RTOS Task
taskHandle_t createTask(void (*taskFunc)(void*), const char *name, uint32_t stack_words, void *param, uint32_t priority);

// Create a delay
void taskDelay(uint32_t ticks);

// Suspend a RTOS Task
void taskSuspend(taskHandle_t task);

// Resume a RTOS Task
void taskResume(taskHandle_t task);

// Exit a RTOS Task
void taskExit(void);

// Get Next Task
tcb_t *pGetNextTask(void);

// Add a task to the READY Queue
void addTaskToReadyQueue(tcb_t *task);

// Remove a task from the READY Queue
void removeTaskFromReadyQueue(tcb_t *task);

// Add a task to the DELAY Queue
void addTaskToDelayQueue(tcb_t *task);

// Remove a task from the DELAY Queue
void removeTaskFromDelayQueue(tcb_t *task);

// Move a task to READY queue   
void moveTaskToReady(tcb_t *task);

// Get Tick Count 
uint32_t getTickCount(void);

#endif
