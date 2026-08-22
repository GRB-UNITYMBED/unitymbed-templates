/*
 * UnityMbed starter — N32G455 (Cortex-M4F, 512KB flash / 144KB RAM)
 * Blinks an LED on PA8.
 */
#include "n32g45x.h"

static void delay(volatile uint32_t n) { while (n--) { __NOP(); } }

int main(void) {
    RCC->APB2PCEN |= (1u << 2);                       /* GPIOA clock */
    GPIOA->CFG_HI = (GPIOA->CFG_HI & ~0xFu) | 0x3u;   /* PA8: output push-pull 50MHz */

    while (1) {
        GPIOA->POD ^= (1u << 8);
        delay(1000000);
    }
}
