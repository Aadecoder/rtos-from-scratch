#ifndef STM32F4XX_H
#define STM32F4XX_H

typedef unsigned int   uint32_t;
typedef int            int32_t;
#define NULL ((void *)0)
typedef unsigned short uint16_t;
typedef short          int16_t;
typedef unsigned char  uint8_t;
typedef char           int8_t;
typedef uint32_t       size_t;

#define __I  volatile const
#define __O  volatile
#define __IO volatile

#define PERIPH_BASE           0x40000000UL
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define APB1PERIPH_BASE       (PERIPH_BASE)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)

#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000UL)

#define FLASH_ACR             (*(__IO uint32_t *)(0x40023C00UL))

typedef struct {
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFRL;
    __IO uint32_t AFRH;
} GPIO_TypeDef;

#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    __IO uint32_t AHB3RSTR;
    uint32_t     RESERVED0;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t     RESERVED1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    __IO uint32_t AHB3ENR;
    uint32_t     RESERVED2;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
    uint32_t     RESERVED3[2];
    __IO uint32_t AHB1LPENR;
    __IO uint32_t AHB2LPENR;
    __IO uint32_t AHB3LPENR;
    uint32_t     RESERVED4;
    __IO uint32_t APB1LPENR;
    __IO uint32_t APB2LPENR;
    uint32_t     RESERVED5[2];
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
    uint32_t     RESERVED6[2];
    __IO uint32_t SSCGR;
    __IO uint32_t PLLI2SCFGR;
    uint32_t     RESERVED7;
    __IO uint32_t DCKCFGR;
} RCC_TypeDef;

#define RCC                 ((RCC_TypeDef *) RCC_BASE)

#define RCC_AHB1ENR_GPIOA_EN    (1 << 0)
#define RCC_AHB1ENR_GPIOB_EN    (1 << 1)
#define RCC_AHB1ENR_GPIOC_EN    (1 << 2)
#define RCC_AHB1ENR_GPIOD_EN    (1 << 3)
#define RCC_AHB1ENR_GPIOE_EN    (1 << 4)

#define GPIO_MODER_OUTPUT(x)    (0x01 << ((x) * 2))

#define GPIO_BSRR_SET(x)        (1 << (x))
#define GPIO_BSRR_RESET(x)      (1 << ((x) + 16))

#define STK_CTRL            (*(__IO uint32_t *)(0xE000E010UL))
#define STK_LOAD            (*(__IO uint32_t *)(0xE000E014UL))
#define STK_VAL             (*(__IO uint32_t *)(0xE000E018UL))

#define STK_CTRL_ENABLE     (1 << 0)
#define STK_CTRL_TICKINT    (1 << 1)
#define STK_CTRL_CLKSOURCE  (1 << 2)
#define STK_CTRL_COUNTFLAG  (1 << 16)

#define SCB_ICSR            (*(__IO uint32_t *)(0xE000ED04UL))
#define SCB_VTOR            (*(__IO uint32_t *)(0xE000ED08UL))
#define SCB_SHPR3           (*(__IO uint32_t *)(0xE000ED20UL))
#define SCB_CPACR           (*(__IO uint32_t *)(0xE000ED88UL))

#define ICSR_PENDSVSET      (1 << 28)
#define ICSR_PENDSVCLR      (1 << 27)

#define SHPR3_PRI_15_14     (0xFF << 16)

#define NVIC_ISER0          (*(__IO uint32_t *)(0xE000E100UL))
#define NVIC_ICER0          (*(__IO uint32_t *)(0xE000E180UL))
#define NVIC_ISPR0          (*(__IO uint32_t *)(0xE000E200UL))
#define NVIC_ICPR0          (*(__IO uint32_t *)(0xE000E280UL))

static inline void __disable_irq(void) { __asm volatile("CPSID I" : : : "memory"); }
static inline void __enable_irq(void)  { __asm volatile("CPSIE I" : : : "memory"); }
static inline uint32_t __get_primask(void) {
    uint32_t result;
    __asm volatile("MRS %0, PRIMASK" : "=r" (result));
    return result;
}
static inline void __set_primask(uint32_t mask) {
    __asm volatile("MSR PRIMASK, %0" : : "r" (mask) : "memory");
}

static inline void __set_psp(uint32_t addr) {
    __asm volatile("MSR PSP, %0" : : "r" (addr) : "memory");
}
static inline uint32_t __get_psp(void) {
    uint32_t result;
    __asm volatile("MRS %0, PSP" : "=r" (result));
    return result;
}
static inline void __set_control(uint32_t control) {
    __asm volatile("MSR CONTROL, %0" : : "r" (control) : "memory");
}

#define portSYSTICK_PRIORITY    0xF0
#define portNVIC_SYSTICK_CTRL   STK_CTRL
#define portNVIC_SYSTICK_LOAD   STK_LOAD
#define portNVIC_SYSTICK_VAL    STK_VAL
#define portNVIC_INT_CTRL       SCB_ICSR
#define portNVIC_PENDSVSET_BIT  ICSR_PENDSVSET
#define portNVIC_PENDSVCLR_BIT  ICSR_PENDSVCLR
#define portNVIC_SYSPRI3        SCB_SHPR3
#define portNVIC_SYSPRI3_MASK   SHPR3_PRI_15_14

#define configCPU_CLOCK_HZ      168000000UL
#define configTICK_RATE_HZ      1000UL
#define configSYSTICK_CLOCK_DIV 1UL

#define portMAX_DELAY           0xFFFFFFFFUL

#endif
