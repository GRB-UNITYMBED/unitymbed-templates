/* UnityMbed starter — Infineon XMC1100 (XMC 2Go / Boot Kit)
 * LED P1.4 / P1.5 กะพริบสลับกัน — bare-metal ล้วน ไม่ใช้ XMClib/DAVE
 *
 * กฎเหล็ก XMC1000 (วัดจากบอร์ดจริง):
 *  1. User flash เริ่มที่ 0x10001000 — ไม่ใช่ 0x0 (linker.ld ตั้งให้แล้ว)
 *  2. Vector slot 0x10/0x14 คือ CLK_VAL1/CLK_VAL2 ที่ Boot ROM อ่านตอนบูต
 *     (0xFFFFFFFF = ใช้ clock default 8 MHz) — ห้ามวาง handler ทับ!
 *  3. GPIO: OMR bit n = set Pn, bit n+16 = reset Pn, ทั้งคู่พร้อมกัน = toggle;
 *     IOCRx หนึ่ง byte ต่อขา, PC field = bits [7:3] (0x80 = push-pull output)
 */
#include <stdint.h>

#define P1_OMR   (*(volatile uint32_t *)0x40040104u)
#define P1_IOCR4 (*(volatile uint32_t *)0x40040114u) /* คุม P1.4..P1.7 */

#define LED1 4u /* P1.4 */
#define LED2 5u /* P1.5 */

static void delay(volatile uint32_t n) {
  while (n--) __asm volatile("nop");
}

int main(void) {
  /* P1.4 + P1.5 → push-pull output */
  P1_IOCR4 = (P1_IOCR4 & 0xFFFF0000u) | 0x00008080u;

  P1_OMR = (1u << LED1) | (1u << (LED2 + 16)); /* เริ่ม: LED1 ติด LED2 ดับ */
  for (;;) {
    /* set+reset พร้อมกันทั้งสองขา = toggle ทั้งคู่ → กะพริบสลับ */
    P1_OMR = (1u << LED1) | (1u << (LED1 + 16)) | (1u << LED2) | (1u << (LED2 + 16));
    delay(400000);
  }
}

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;

void Reset_Handler(void) {
  uint32_t *src = &_sidata, *dst = &_sdata;
  while (dst < &_edata) *dst++ = *src++;
  for (dst = &_sbss; dst < &_ebss;) *dst++ = 0;
  main();
  for (;;) {}
}

void Default_Handler(void) { for (;;) {} }

typedef void (*isr_t)(void);
__attribute__((section(".vectors"), used)) static const isr_t vectors[] = {
    (isr_t)&_estack,
    Reset_Handler,
    Default_Handler,        /* NMI */
    Default_Handler,        /* HardFault */
    (isr_t)0xFFFFFFFFu,     /* 0x10: CLK_VAL1 — Boot ROM config, erased = default */
    (isr_t)0xFFFFFFFFu,     /* 0x14: CLK_VAL2 — Boot ROM config, erased = default */
};
