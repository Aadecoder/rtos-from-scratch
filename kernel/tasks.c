#include "../Inc/tasks.h"
#include "../Inc/types.h"
#include "../Inc/port.h"
#include <stdint.h>


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

}

void create_task(tcb_t* taskHandler, void* taskFunc(void*)){
	// Automatically stored
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x01000000; 				// xPSR
	taskHandler->sp -= 1;
	*taskHandler->sp = (uint32_t)taskFunc | 1; 	// PC
	taskHandler->sp -= 1;
	*taskHandler->sp = 0xFFFFFFFD; 				// LR
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x12121212; 				// R12
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x03030303; 				// R3
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x02020202; 				// R2
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x01010101; 				// R1
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x00000000; 				// R0

	// Manually Stored
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x11111111; 				// R11
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x10101010; 				// R10
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x09090909; 				// R9
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x08080808; 				// R8
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x07070707; 				// R7
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x06060606; 				// R6
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x05050505; 				// R5
	taskHandler->sp -= 1;
	*taskHandler->sp = 0x04040404; 				// R4
}

