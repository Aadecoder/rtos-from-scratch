#include "../Inc/tasks.h"
#include "../Inc/types.h"
#include "../Inc/port.h"
#include "../Inc/scheduler.h"


tcb_t taskArray[MAX_TASKS];
uint32_t taskCount = 0;
tcb_t *pCurrentTcb = NULL;
tcb_t *pDelayedQueue = NULL;
static uint32_t tickCount = 0;

static tcb_t *pAllocateTcb(void){
    for (uint32_t i=0; i < MAX_TASKS; i++){
        if (taskArray[i].state == SUSPENDED && taskArray[i].task_id == 0){
            taskArray[i].task_id = i+1;
            return &taskArray[i];
        }
    }
    return NULL;
}

static void taskInitTcb(tcb_t *tcb){
    tcb->sp = NULL;
    tcb->priority = 0;
    tcb->base_priority = 0;
    tcb->state = SUSPENDED;
    tcb->delay_ticks = 0;
    tcb->name = NULL;
    tcb->rdy_next = NULL;
    tcb->rdy_prev = NULL;
    tcb->dl_next = NULL;
    tcb->dl_prev = NULL;
    tcb->wt_next = NULL;
    tcb->wait_obj = NULL;
    tcb->recursive_count = 0;
}

void *pPortMalloc(uint32_t size){
    static uint8_t heap[4096];
    static uint32_t heapOffset = 0;
    uint32_t aligned = (size + 3) & ~3;
    
    if (heapOffset + aligned > sizeof(heap)){
        return NULL;
    }

    void *ptr = &heap[heapOffset];
    heapOffset += aligned;
    return ptr;
}

void taskExit(void){
    __disable_irq();
    while(1){
        __asm volatile("WFI");
    }
}

taskHandle_t createTask(void (*taskFunc)(void*), const char *name, uint32_t stack_words, void *param, uint32_t priority){
    uint32_t mask;
    tcb_t *tcb;
    uint32_t *stack;

    if (priority >= MAX_PRIORITIES){
        priority = MAX_PRIORITIES - 1;
    }

        stack_words = MIN_STACK_SIZE;
    if (stack_words < MIN_STACK_SIZE){
    }

    tcb = pAllocateTcb();
    if (tcb == NULL){
        exitCritical(mask);
        return NULL;
    }

    stack = (uint32_t *)pPortMalloc(stack_words * sizeof(uint32_t));
    if (stack == NULL){
        tcb->task_id = 0;
        exitCritical(mask);
        return NULL;
    }

    for(uint32_t i = 0; i < stack_words; i++){
        stack[i] = STACK_FILL_PATTERN;
    }

    uint32_t *sp = stack + stack_words;
    sp -= 16;

    sp[8] = (uint32_t)param;
    sp[9] = 0;
    sp[10] = 0;
    sp[11] = 0;
    sp[12] = 0;
    sp[13] = (uint32_t)taskExit;
    sp[14] = (uint32_t) taskFunc;
    sp[15] = 0x01000000;

    tcb->sp = sp;
    tcb->priority = priority;
    tcb->base_priority = priority;
    tcb->state = READY;
    tcb->name = name;
    tcb->delay_ticks = 0;

    addTaskToReadyQueue(tcb);
    taskCount++;

    if (pCurrentTcb && priority > pCurrentTcb->priority){
        portNVIC_INT_CTRL = portNVIC_PENDSVSET_BIT;
    }

    exitCritical(mask);
    return tcb;
}

void taskDelay(uint32_t ticks){
    uint32_t mask;

    if (ticks ==  0){
        taskYield();
        return;
    }

    mask = enterCritical();

    pCurrentTcb->delay_ticks = ticks;
    removeTaskFromReadyQueue(pCurrentTcb);
    pCurrentTcb->state = BLOCKED;
    addTaskToDelayQueue(pCurrentTcb);
    exitCritical(mask);

    taskYield();
}

void taskSuspend(taskHandle_t task){
    uint32_t mask = enterCritical();

    if (task == NULL){
        task = pCurrentTcb;
    }

    if (task->state == READY || task->state == RUNNING){
        removeTaskFromReadyQueue(task);
    }else if (task->state == BLOCKED){
        removeTaskFromDelayQueue(task);
    }

    task->state = SUSPENDED;

    exitCritical(mask);

    if (task == pCurrentTcb){
        taskYield();
    }
}

void taskResume(taskHandle_t task){
    uint32_t mask = enterCritical();

    if (task == NULL || task->state != SUSPENDED){
        exitCritical(mask);
        return;
    }

    task->state = READY;
    addTaskToReadyQueue(task);

    if (task->priority > pCurrentTcb->priority){
        portNVIC_INT_CTRL = portNVIC_PENDSVSET_BIT;
    }
    exitCritical(mask);
}

void addTaskToReadyQueue(tcb_t *task){
    uint32_t priority = task->priority;
    tcbQueue_t *queue = &readyQueue[priority];

    if (queue->head == NULL){
        queue->head = task;
        queue->tail = task;
        task->rdy_next = NULL;
        task->rdy_prev = NULL;
        readyBitMap |= (1 << priority);
    }else {
        queue->tail->rdy_next = task;
        task->rdy_prev = queue->tail;
        task->rdy_next = NULL;
        queue->tail = task;
    }
}

void removeTaskFromReadyQueue(tcb_t *task){
    uint32_t priority = task->priority;
    tcbQueue_t *queue = &readyQueue[priority];

    if (task->rdy_prev){
        task->rdy_prev->rdy_next = task->rdy_next;
    } else{
        queue->head = task->rdy_next;
    }

    if (task->rdy_next){
        task->rdy_next->rdy_prev = task->rdy_prev;
    } else{
        queue->tail = task->rdy_prev;
    }

    if (queue->head == NULL){
        readyBitMap &= ~(1 << priority);
    }
}

void addTaskToDelayQueue(tcb_t *task){
    tcb_t *prev = NULL;
    tcb_t *curr = pDelayedQueue;

    while (curr != NULL &&  curr->delay_ticks <= task->delay_ticks){
        prev = curr;
        curr = curr->dl_next;
    }

    task->dl_next = curr;
    task->dl_prev = prev;

    if (prev){
        prev->dl_next = task;
    }else{
        pDelayedQueue = task;
    }

    if (curr){
        curr->dl_prev = task;
    }
}

void removeTaskFromDelayQueue(tcb_t *task){
    if (task->dl_prev){
        task->dl_prev->dl_next = task->dl_next;
    }else{
        pDelayedQueue = task->rdy_next;
    }

    if (task->dl_next){
        task->dl_next->dl_prev = task->dl_prev;
    }

    task->dl_next = NULL;
    task->dl_prev = NULL;
    task->delay_ticks = 0;
}

void moveTaskToReady(tcb_t *task){
    if (task->state == BLOCKED){
        removeTaskFromDelayQueue(task);
    }

    if (task->wt_next || task->wait_obj){
        task->wt_next = NULL;
        task->wait_obj = NULL;
    }

    task->state = READY;
    addTaskToReadyQueue(task);
}

uint32_t getTickCount(void){
    return tickCount;
}

void taskIncrementTick(void){
    tcb_t *task = pDelayedQueue;
    tcb_t *next;

    tickCount++;

    while (task != NULL){
        next = task->dl_next;
        if (task->delay_ticks > 0){
            task->delay_ticks--;
        }
        if (task->delay_ticks == 0){
            moveTaskToReady(task);
        }else{
            break;
        }
        task = next;
    }
}
