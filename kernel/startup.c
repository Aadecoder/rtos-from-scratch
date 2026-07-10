#include <stdint.h>

// Forward declarations
extern uint32_t _estack;      // defined in linker script
extern int main(void);
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void Systick_Handler(void);
void Reset_Handler(void);
void Default_Handler(void);

// Minimal vector table (Cortex-M3 style)
__attribute__((section(".isr_vector")))
uint32_t vectors[] = {
    (uint32_t)&_estack,        // Initial stack pointer
    (uint32_t)Reset_Handler,   // Reset
    (uint32_t)Default_Handler, // NMI
    (uint32_t)Default_Handler, // HardFault
    (uint32_t)Default_Handler, // MemManage
    (uint32_t)Default_Handler, // BusFault
    (uint32_t)Default_Handler, // UsageFault
    0,0,0,0,                   // Reserved
    (uint32_t)SVC_Handler, // SVC
    (uint32_t)Default_Handler, // DebugMon
    0,                          // Reserved
    (uint32_t)PendSV_Handler, // PendSV
    (uint32_t)Systick_Handler, // SysTick
};

void Reset_Handler(void) {
    // Copy .data from flash to RAM, zero .bss (if needed)
    // For simplicity, we'll do nothing for now.
    main();
    while(1);
}

void Default_Handler(void) {
    while(1);
}
