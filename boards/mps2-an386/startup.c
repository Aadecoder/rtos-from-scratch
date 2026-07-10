#include <stdint.h>

extern uint32_t _estack;
extern void Reset_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
extern void Default_Handler(void);

// Vector table - must be first in flash
__attribute__((section(".vector_table")))
uint32_t vectors[] = {
    (uint32_t)&_estack,         // initial stack pointer
    (uint32_t)Reset_Handler,    // reset
    (uint32_t)Default_Handler,  // NMI
    (uint32_t)Default_Handler,  // HardFault
    (uint32_t)Default_Handler,  // MemManage
    (uint32_t)Default_Handler,  // BusFault
    (uint32_t)Default_Handler,  // UsageFault
    0, 0, 0, 0,                 // reserved
    (uint32_t)Default_Handler,  // SVCall
    0, 0,                       // reserved
    (uint32_t)PendSV_Handler,   // PendSV
    (uint32_t)SysTick_Handler,  // SysTick
};

extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

void Reset_Handler(void) {
    // copy .data from flash to RAM
    uint32_t *src = &_sidata, *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    // zero out .bss
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    extern void main(void);
    main();
}

void Default_Handler(void) {
    while(1);  // hang so you can catch it in GDB
}
