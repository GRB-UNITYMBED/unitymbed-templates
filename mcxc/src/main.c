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
#define PORT_PCR(port, n) (*(volatile uint32_t *)(0x40049000u + 0x1000u * (port) + 4u * (n)))
#define GPIO_PSOR(port) (*(volatile uint32_t *)(0x400FF004u + 0x40u * (port)))
#define GPIO_PCOR(port) (*(volatile uint32_t *)(0x400FF008u + 0x40u * (port)))
#define GPIO_PDDR(port) (*(volatile uint32_t *)(0x400FF014u + 0x40u * (port)))

/* LED map ต่อบอร์ด (Makefile เลือก define ตาม SOC=) — active-low ทั้งคู่
 *   FRDM-MCXC041: RGB = PTB10 (R) / PTB11 (G) / PTB13 (B)
 *   FRDM-MCXC444: RGB = PTE31 (R) / PTE29 (G) / PTD5 (B) — วัดจากบอร์ดจริง */
#ifdef UM_BOARD_MCXC444
#define PORT_R 4
#define PIN_R 31
#define PORT_G 4
#define PIN_G 29
#define PORT_B 3
#define PIN_B 5
#define SCGC5_MASK ((1u << 12) | (1u << 13)) /* PORTD + PORTE */
#else
#define PORT_R 1
#define PIN_R 10
#define PORT_G 1
#define PIN_G 11
#define PORT_B 1
#define PIN_B 13
#define SCGC5_MASK (1u << 10) /* PORTB */
#endif

static void delay(volatile uint32_t n) { while (n--) __asm volatile(""); }

int main(void) {
  SIM_SCGC5 |= SCGC5_MASK; /* เปิด clock พอร์ตก่อนแตะ PORTx เสมอ */
  PORT_PCR(PORT_R, PIN_R) = 0x100; /* MUX=1: GPIO */
  PORT_PCR(PORT_G, PIN_G) = 0x100;
  PORT_PCR(PORT_B, PIN_B) = 0x100;
  GPIO_PSOR(PORT_R) = 1u << PIN_R; /* active-low: set = ดับ */
  GPIO_PSOR(PORT_G) = 1u << PIN_G;
  GPIO_PSOR(PORT_B) = 1u << PIN_B;
  GPIO_PDDR(PORT_R) |= 1u << PIN_R;
  GPIO_PDDR(PORT_G) |= 1u << PIN_G;
  GPIO_PDDR(PORT_B) |= 1u << PIN_B;
  for (;;) {
    GPIO_PCOR(PORT_R) = 1u << PIN_R; delay(200000); GPIO_PSOR(PORT_R) = 1u << PIN_R;
    GPIO_PCOR(PORT_G) = 1u << PIN_G; delay(200000); GPIO_PSOR(PORT_G) = 1u << PIN_G;
    GPIO_PCOR(PORT_B) = 1u << PIN_B; delay(200000); GPIO_PSOR(PORT_B) = 1u << PIN_B;
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
