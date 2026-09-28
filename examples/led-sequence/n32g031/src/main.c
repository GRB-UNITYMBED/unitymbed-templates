// UnityMbed — blink LED on PB1, PB3, PB6, PB7 @ N32G031
#include "n32g031.h"

static void delay(volatile uint32_t n) { while (n--) __NOP(); }

int main(void) {
    /* Enable GPIOB clock via APB2 bit 3 */
    RCC->APB2PCLKEN |= (1u << 3);

    /* Configure PB1, PB3, PB6, PB7 as push-pull output */
    // Clear PMODE bits for PB1[3:2], PB3[7:6], PB6[13:12], PB7[15:14]
    GPIOB->PMODE &= ~((0x3U << 2) | (0x3U << 6) | (0x3U << 12) | (0x3U << 14));
    // Set PMODE to 01 (General purpose output)
    GPIOB->PMODE |=  ((0x1U << 2) | (0x1U << 6) | (0x1U << 12) | (0x1U << 14)); 
    
    // Clear POTYPE bits to 0 (Push-pull) for PB1, PB3, PB6, PB7
    GPIOB->POTYPE &= ~((1U << 1) | (1U << 3) | (1U << 6) | (1U << 7));                                 
    
    // Turn off all LEDs initially (Write 1 to upper 16 bits of PBSC to clear)
    GPIOB->PBSC = (1U << (1 + 16)) | (1U << (3 + 16)) | (1U << (6 + 16)) | (1U << (7 + 16));

    while (1) {
        // PB1 ON
        GPIOB->PBSC = (1U << 1);
        delay(400000);
        GPIOB->PBSC = (1U << (1 + 16)); // PB1 OFF
        
        // PB3 ON
        GPIOB->PBSC = (1U << 3);
        delay(400000);
        GPIOB->PBSC = (1U << (3 + 16)); // PB3 OFF
        
        // PB6 ON
        GPIOB->PBSC = (1U << 6);
        delay(400000);
        GPIOB->PBSC = (1U << (6 + 16)); // PB6 OFF
        
        // PB7 ON
        GPIOB->PBSC = (1U << 7);
        delay(400000);
        GPIOB->PBSC = (1U << (7 + 16)); // PB7 OFF
    }
}