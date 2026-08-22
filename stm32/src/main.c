/*
 * UnityMbed STM32 starter — bare metal, no HAL.
 * Blinks the Nucleo user LED. Ask the AI to adapt pins/peripherals for your
 * exact part — it knows every STM32 line natively (see "stm32" info that
 * `unitymbed init` recorded in unitymbed.json).
 *
 * Default pin: PB3 (Nucleo-32 boards). Nucleo-64 uses PA5 — two lines to change.
 */
#include <stdint.h>

#define RCC_AHBENR (*(volatile uint32_t *)0x40021014) /* F0/F1/F3-style */
#define GPIOB_MODER (*(volatile uint32_t *)0x48000400)
#define GPIOB_BSRR (*(volatile uint32_t *)0x48000418)
#define LED_PIN 3u

#define SYST_CSR (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR (*(volatile uint32_t *)0xE000E018)

static void delay_ms(uint32_t ms) {
  SYST_RVR = 8000u - 1u; /* HSI 8 MHz หลัง reset */
  SYST_CVR = 0u;
  SYST_CSR = 5u;
  while (ms--) {
    while (!(SYST_CSR & (1u << 16))) {
    }
  }
  SYST_CSR = 0u;
}

int main(void) {
  RCC_AHBENR |= (1u << 18); /* GPIOB clock (bit17 = GPIOA สำหรับบอร์ด PA5) */
  GPIOB_MODER = (GPIOB_MODER & ~(3u << (LED_PIN * 2))) | (1u << (LED_PIN * 2));
  for (;;) {
    GPIOB_BSRR = (1u << LED_PIN);
    delay_ms(500);
    GPIOB_BSRR = (1u << (LED_PIN + 16));
    delay_ms(500);
  }
}

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;
void Reset_Handler(void) {
  uint32_t *src = &_sidata, *dst = &_sdata;
  while (dst < &_edata) *dst++ = *src++;
  for (dst = &_sbss; dst < &_ebss;) *dst++ = 0;
  main();
  for (;;) {
  }
}
void Default_Handler(void) {
  for (;;) {
  }
}
__attribute__((section(".vectors"), used)) static const uint32_t vectors[] = {
    (uint32_t)&_estack,
    (uint32_t)Reset_Handler,
    (uint32_t)Default_Handler,
    (uint32_t)Default_Handler,
};
