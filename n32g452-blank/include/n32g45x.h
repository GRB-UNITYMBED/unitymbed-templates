#ifndef N32G45X_H
#define N32G45X_H

/*
 * N32G45x register map — F1-style layout, verified on real N32G455 silicon
 * (2026-08-23, debugger read-back on live board). The N32G45x is NOT an
 * F4-style part: GPIO lives on APB2 at 0x40010800 with CFG_LO/CFG_HI
 * nibble-per-pin config, and RCC lives at 0x40021000.
 */

#include <stdint.h>

#define __NOP() __asm volatile("nop")

typedef struct {
    volatile uint32_t CTRL;      /* 0x00 clock control */
    volatile uint32_t CFG;       /* 0x04 clock config */
    volatile uint32_t CLKINT;    /* 0x08 */
    volatile uint32_t APB2PRST;  /* 0x0C */
    volatile uint32_t APB1PRST;  /* 0x10 */
    volatile uint32_t AHBPCLKEN; /* 0x14 */
    volatile uint32_t APB2PCEN;  /* 0x18 bit0 AFIO, bit2 IOPA, bit3 IOPB, bit4 IOPC */
    volatile uint32_t APB1PCEN;  /* 0x1C */
} RCC_TypeDef;

typedef struct {
    volatile uint32_t CFG_LO;    /* 0x00 pins 0..7,  nibble/pin: 0x3 = out PP 50MHz */
    volatile uint32_t CFG_HI;    /* 0x04 pins 8..15 */
    volatile uint32_t PID;       /* 0x08 input data (ขาจริง) */
    volatile uint32_t POD;       /* 0x0C output data */
    volatile uint32_t PBSC;      /* 0x10 bit n = set, bit n+16 = clear */
    volatile uint32_t PBC;       /* 0x14 bit n = clear */
    volatile uint32_t PLOCK_CFG; /* 0x18 */
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CFG;         /* 0x00 event output */
    volatile uint32_t RMP_CFG;     /* 0x04 remap; SWJ_CFG[26:24]: 010 = JTAG off/SWD on
                                            (จำเป็นถ้าจะใช้ PB3/PB4/PA15 เป็น GPIO) */
    volatile uint32_t EXTI_CFG[4]; /* 0x08..0x14 */
} AFIO_TypeDef;

#define RCC   ((RCC_TypeDef *)  0x40021000UL)
#define AFIO  ((AFIO_TypeDef *) 0x40010000UL)
#define GPIOA ((GPIO_TypeDef *) 0x40010800UL)
#define GPIOB ((GPIO_TypeDef *) 0x40010C00UL)
#define GPIOC ((GPIO_TypeDef *) 0x40011000UL)
#define GPIOD ((GPIO_TypeDef *) 0x40011400UL)

#endif
