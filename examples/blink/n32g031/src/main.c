#include "n32g031.h"
#include "utils.h"

static void gpio_init(void) {
    // Enable GPIOB clock (Bit 3 in APB2PCLKEN)
    RCC->APB2PCLKEN |= (1U << 3);

    // Configure PB7 as General Purpose Output (PMODE7 = 01)
    GPIOB->PMODE = (GPIOB->PMODE & ~(0x3U << (7 * 2))) | (0x1U << (7 * 2));

    // Configure PB7 as Push-Pull output (POTYPE7 = 0)
    GPIOB->POTYPE &= ~(1U << 7);
}

int main(void) {
    // Initialize GPIO for the LED
    gpio_init();
    
    while (1) {
        // Toggle PB7 output state
        GPIOB->POD ^= (1U << 7);
        
        // Wait for 500 ms
        delay_ms(500);
    }
}