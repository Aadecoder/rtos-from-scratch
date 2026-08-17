#ifndef TYPES_H
#define TYPES_H
#include "stm32f4xx.h"

typedef enum{
    UNUSED,
	READY,
	BLOCKED,
	RUNNING,
    SUSPENDED,
}state;

typedef struct tcb{
    uint32_t *sp;
    uint32_t priority;
    uint32_t base_priority;
    uint32_t state;
    uint32_t delay_ticks;
    const char *name;
    struct tcb *rdy_next;
    struct tcb *rdy_prev;
    struct tcb *dl_next;
    struct tcb *dl_prev;
    struct tcb *wt_next;
    void *wait_obj;
    uint32_t recursive_count;
    uint32_t task_id;
}tcb_t;

typedef tcb_t *taskHandle_t;

#endif
