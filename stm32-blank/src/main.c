/*
 * UnityMbed STM32 starter — bare metal, no HAL.
 * Blank project: no clocks, GPIO or peripherals configured. Ask the AI to
 * add whatever your part needs — it knows every STM32 line natively (see
 * "stm32" info that `unitymbed init` recorded in unitymbed.json).
 */
#include <stdint.h>

int main(void) {
  for (;;) {
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
