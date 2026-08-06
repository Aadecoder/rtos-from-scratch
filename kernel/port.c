#include "../Inc/port.h"

uint32_t enterCritical(void){
    uint32_t mask = __get_primask();
    __disable_irq();
    return mask;
}

void exitCritical(uint32_t mask){
    __set_primask(mask);
}

void setupTimerInterrupt(void){
    uint32_t reload = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1;
    portNVIC_SYSTICK_CTRL = 0;
    portNVIC_SYSTICK_LOAD = reload;
    portNVIC_SYSTICK_VAL = 0;

    __disable_irq();
    portNVIC_SYSPRI3 = (portNVIC_SYSPRI3 & ~portNVIC_SYSPRI3_MASK) | (portSYSTICK_PRIORITY << 16);

    portNVIC_SYSTICK_CTRL = STK_CTRL_CLKSOURCE | STK_CTRL_TICKINT | STK_CTRL_ENABLE;

    __enable_irq();
}
