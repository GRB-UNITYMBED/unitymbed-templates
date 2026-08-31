/* MAX78000FTHR — bare metal blink, no SDK.
 *
 * Every address below is transcribed from the vendor's own headers
 * (analogdevicesinc/msdk), not inferred:
 *   LPGCR base 0x40080000, pclkdis at 0x0C, GPIO2 is bit 0   (lpgcr_regs.h)
 *   GPIO2 0x40080400 — NOT 0x4000A000. It does not continue the
 *     GPIO0/GPIO1 spacing; it sits in the low-power domain beside
 *     LPGCR, which is also what gates its clock.        (max78000.h)
 *   en0 +0x00, outen +0x0C, out_set +0x1C, out_clr +0x20     (gpio_regs.h)
 *   LEDs are GPIO2 pins 0,1,2                                (FTHR board.c)
 *
 * The stack pointer is not a guess either: the firmware already on this
 * board carries 0x20020000 in its vector table, read back over SWD.
 */
#include <stdint.h>

#define LPGCR_PCLKDIS (*(volatile uint32_t *)(0x40080000u + 0x0Cu))
#define GPIO2_EN0     (*(volatile uint32_t *)(0x40080400u + 0x00u))
#define GPIO2_OUTEN   (*(volatile uint32_t *)(0x40080400u + 0x0Cu))
#define GPIO2_OUT_SET (*(volatile uint32_t *)(0x40080400u + 0x1Cu))
#define GPIO2_OUT_CLR (*(volatile uint32_t *)(0x40080400u + 0x20u))

#define LED_RED   (1u << 0)
#define LED_GREEN (1u << 1)
#define LED_BLUE  (1u << 2)
#define LEDS      (LED_RED | LED_GREEN | LED_BLUE)

static void spin(volatile uint32_t n) {
  while (n--) __asm__ volatile("nop");
}

int main(void) {
  /* PCLKDIS is a *disable* register — clearing the bit turns the clock on. */
  LPGCR_PCLKDIS &= ~1u;

  GPIO2_EN0 |= LEDS;   /* GPIO mode rather than an alternate function */
  GPIO2_OUTEN |= LEDS; /* drive them */

  for (;;) {
    GPIO2_OUT_CLR = LED_GREEN; /* FTHR LEDs are active low */
    spin(1500000);
    GPIO2_OUT_SET = LED_GREEN;
    spin(1500000);
  }
}

extern uint32_t _estack;
void Reset_Handler(void) {
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
