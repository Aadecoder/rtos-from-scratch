#include <stdint.h>

#define NUM_OF_THREADS 2
#define STACKSIZE 256

void yield(void);

// TCB (Task Control Block)
typedef struct tcb_t{
    int32_t *stackPt;
    struct tcb_t *nextPt;
}tcb_t;

// An array to store the TCB as tasks
tcb_t tcbs[NUM_OF_THREADS];
// current task
tcb_t *pcurntTcb;
// stack for each task
int32_t TCB_STACK[NUM_OF_THREADS][STACKSIZE];

void kernelInit(void){
    for (int i=0;i<NUM_OF_THREADS;i++){
        tcbs[i].stackPt = &TCB_STACK[i][STACKSIZE-1];
    }
    tcbs[0].nextPt = &tcbs[1];
    tcbs[1].nextPt = &tcbs[0];
}

// Task Functions
void task1(void){
    while(1){
        yield();
    }
}

void task2(void){
    while(1){
        yield();
    }
}

void task_init(tcb_t *taskHandler, void (*taskFunc)(void)){

    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x01000000; // xPSR (Setting the thumb bit)
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = (uint32_t)taskFunc | 1; // PC
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0xFFFFFFFD; // LR
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x12121212; // R12
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x03030303; // R3
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x02020202; // R2
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x01010101; // R1
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x22222222; // R0
    taskHandler->stackPt -= 1;

    *taskHandler->stackPt = 0x11111111; // R11
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x10101010; // R10
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x09090909; // R9
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x08080808; // R8
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x07070707; // R7
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x06060606; // R6
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x05050505; // R5
    taskHandler->stackPt -= 1;
    *taskHandler->stackPt = 0x04040404; // R4
}

__attribute__((naked))void PendSV_Handler(void){
    // Save the current task context
    // Push registers R4 to R11 to PSP
    __asm("MRS R0, PSP"); // read PSP of currect task into R0
    __asm("STMDB R0!, {r4-r11}"); // push r4-r11 onto psp using r0 as the pointer
    // Point R1 to the address of the pointer to current task
    __asm("LDR R1, =pcurntTcb");
    // load R1 with the actual address of the task now that we dereference it
    __asm("LDR R1, [R1]");
    // store the value of the new stack ptr which R0 points to in the current tasks "stackPt element in its TCB"
    __asm("STR R0, [R1]");

    // load stuff of next task
    // Reads the nextPt field of curr tcb
    __asm("LDR R1, [R1,#4]");
    // point R2 to the address of the pcurntTcb
    __asm("LDR R2, =pcurntTcb");
    // Write the new TCB Address from R1 into the global variable
    __asm("STR R1, [R2]");
    // load the next tasks stack pointer in R0, this reads the stackPt field of next TCB address stored in R1 and stores in R0
    __asm("LDR R0, [R1]");
    // Pop r4-r11 off the next tasks stack using R0
    __asm("LDMIA R0!, {r4-r11}");
    // Write the updated address back into PSP Register
    __asm("MSR PSP, R0");
    // exit the handler
    __asm("BX LR");
}

__attribute__((naked)) void SVC_Handler(void) {
    __asm volatile (
        "LDR R0, =pcurntTcb     \n"
        "LDR R0, [R0]           \n"   // R0 = pcurntTcb (current task TCB)
        "LDR R0, [R0]           \n"   // R0 = stackPt
        "ADD R0, R0, #32        \n"   // point to exception frame
        "MSR PSP, R0            \n"
        "MRS R0, CONTROL        \n"
        "ORR R0, R0, #2         \n"   // set SPSEL
        "MSR CONTROL, R0        \n"
        "ISB                    \n"
        "LDR LR, =0xFFFFFFFD    \n"
        "BX LR                  \n"
    );
}

void Systick_Handler(void){
    // Trigger PendSV (set bit 28 of ICSR = 0xE000ED04)
    *(volatile uint32_t *)0xE000ED04 = (1 << 28);
}

void yield(void){
    Systick_Handler();
}

int main(void){
    kernelInit();
    task_init(&tcbs[0], task1);
    task_init(&tcbs[1], task2);
    pcurntTcb = &tcbs[0];
    __asm volatile ("SVC #0");   // triggers SVC exception
    while(1);
}
