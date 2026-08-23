/* UnityMbed starter — NXP MCXC041 (FRDM-MCXC041, Kinetis KL03 die)
 * RGB LED กะพริบวน: PTB10 (แดง) -> PTB11 (เขียว) -> PTB13 (น้ำเงิน), active-low
 *
 * กฎเหล็ก KL03/MCXC:
 *  1. SIM_COPC = 0 ให้เร็วที่สุดหลังบูต (write-once) — ไม่งั้น COP watchdog
 *     รีเซ็ตทุก ~1 วินาที
 *  2. Flash Config Field ที่ 0x400 ต้องมี FSEC=0xFE (ดู fcf[] ท้ายไฟล์) —
 *     ค่าผิดตัวเดียว = ชิปล็อกถาวร ห้ามแก้ถ้าไม่รู้ว่ากำลังทำอะไร
 *  3. เปิด clock พอร์ตผ่าน SIM_SCGC5 ก่อนแตะ PORTx เสมอ (แตะก่อนเปิด = HardFault) */
#include <stdint.h>

#define SIM_COPC     (*(volatile uint32_t *)0x40048100u)
#define SIM_SCGC5    (*(volatile uint32_t *)0x40048038u)
#define PORTB_PCR(n) (*(volatile uint32_t *)(0x4004A000u + 4u * (n)))
#define GPIOB_PSOR   (*(volatile uint32_t *)0x400FF044u)
#define GPIOB_PCOR   (*(volatile uint32_t *)0x400FF048u)
#define GPIOB_PDDR   (*(volatile uint32_t *)0x400FF054u)

#define LED_R (1u << 10)
#define LED_G (1u << 11)
#define LED_B (1u << 13)

static void delay(volatile uint32_t n) { while (n--) __asm volatile(""); }

int main(void) {
  SIM_SCGC5 |= (1u << 10); /* clock PORTB */
  PORTB_PCR(10) = 0x100;   /* MUX=1: GPIO */
  PORTB_PCR(11) = 0x100;
  PORTB_PCR(13) = 0x100;
  GPIOB_PSOR = LED_R | LED_G | LED_B; /* active-low: set = ดับ */
  GPIOB_PDDR |= LED_R | LED_G | LED_B;
  for (;;) {
    GPIOB_PCOR = LED_R; delay(200000); GPIOB_PSOR = LED_R;
    GPIOB_PCOR = LED_G; delay(200000); GPIOB_PSOR = LED_G;
    GPIOB_PCOR = LED_B; delay(200000); GPIOB_PSOR = LED_B;
  }
}

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;

void Reset_Handler(void) {
  SIM_COPC = 0; /* ปิด watchdog ก่อน copy .data (กันรีเซ็ตกลางทาง) */
  uint32_t *src = &_sidata, *dst = &_sdata;
  while (dst < &_edata) *dst++ = *src++;
  for (dst = &_sbss; dst < &_ebss;) *dst++ = 0;
  main();
  for (;;) {}
}

void Default_Handler(void) { for (;;) {} }

typedef void (*isr_t)(void);
__attribute__((section(".vectors"), used)) static const isr_t vectors[] = {
    (isr_t)&_estack, Reset_Handler, Default_Handler, Default_Handler,
};

/* Flash Configuration Field @0x400 — FSEC=0xFE (unsecured) */
__attribute__((section(".fcf"), used)) static const uint8_t fcf[16] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF,
};
