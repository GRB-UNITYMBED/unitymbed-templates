/*
 * nRF9160 mini secure boot — ด่านที่ 1 ของ agri-sensor PoC
 * ทำหน้าที่เดียว: ตั้ง SPU ให้ flash/RAM/GPIO ฝั่งบน เป็น non-secure
 * แล้วกระโดดไปรัน NS app ที่ 0x40000 (แทน TF-M ของ Zephyr ทั้งก้อน)
 *
 * SPU (secure-only) base 0x50003000:
 *   GPIOPORT[0].PERM  +0x4C0  bit ต่อขา: 1=secure → เขียน 0 = ปล่อยทุกขาให้ NS
 *   FLASHREGION[i].PERM +0x600+4i  (32KB/region) bits: 0=EXEC 1=WRITE 2=READ 4=SECATTR
 *   RAMREGION[i].PERM   +0x700+4i  (8KB/region)
 *   PERM = 0x07 (R|W|X, SECATTR=0) = non-secure เต็มสิทธิ์
 */
#include <stdint.h>

extern void SystemInit(void); /* mdk: errata + FICR trims — ของที่ Zephyr/TF-M เรียกเสมอ */

#define SPU_BASE 0x50003000u
/* nRF9160 จริง: GPIOPORT[0].PERM อยู่ 0x480 (เจอจากสแกน — ค่า reset 0x0000FFFF)
 * เคลียร์ 0x4C0 เผื่อไว้ด้วย (ตำแหน่งของ nRF5340) */
#define SPU_GPIOPORT0 (*(volatile uint32_t *)(SPU_BASE + 0x480))
#define SPU_GPIOPORT0_ALT (*(volatile uint32_t *)(SPU_BASE + 0x4C0))
#define SPU_FLASH(i) (*(volatile uint32_t *)(SPU_BASE + 0x600 + 4u * (i)))
#define SPU_RAM(i) (*(volatile uint32_t *)(SPU_BASE + 0x700 + 4u * (i)))
/* P0 GPIO = peripheral ID 66 — SECATTR (bit4) ตั้งต้นเป็น secure ต้องเคลียร์
 * (เจอจากอ่านจริง: PERM=0x80000013 → NS เขียน GPIO แล้ว BusFault ทันที) */
#define SPU_PERIPHID(i) (*(volatile uint32_t *)(SPU_BASE + 0x800 + 4u * (i)))

#define SAU_CTRL (*(volatile uint32_t *)0xE000EDD0)
#define SCB_NS_VTOR (*(volatile uint32_t *)0xE002ED08)

#define NS_APP_BASE 0x00040000u /* flash region 8 เป็นต้นไป = NS */

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void) {
  uint32_t *src = &_sidata, *dst = &_sdata;
  while (dst < &_edata) *dst++ = *src++;
  for (dst = &_sbss; dst < &_ebss;) *dst++ = 0;
  SystemInit();

  /* flash region 8..31 (0x40000 ขึ้นไป) → non-secure */
  for (uint32_t i = 8; i < 32; i++) SPU_FLASH(i) = 0x07;
  /* RAM region 8..31 (0x20010000 ขึ้นไป) → non-secure */
  for (uint32_t i = 8; i < 32; i++) SPU_RAM(i) = 0x07;
  /* GPIO ทุกขา → non-secure */
  SPU_GPIOPORT0 = 0;
  SPU_GPIOPORT0_ALT = 0;
  SPU_PERIPHID(66) &= ~(1u << 4); /* GPIO peripheral → non-secure */
  /* ชุดที่ modem lib ฝั่ง NS ต้องแตะ: REGULATORS(4), CLOCK/POWER(5), IPC(42) */
  SPU_PERIPHID(4) &= ~(1u << 4);
  SPU_PERIPHID(5) &= ~(1u << 4);
  SPU_PERIPHID(42) &= ~(1u << 4);
  /* modem domain (EXTDOMAIN[0]) → non-secure — ชิ้นที่ TF-M ตั้งให้เสมอ:
   * คุมว่า bus master ของ modem core ถูกมองเป็น NS (จำเป็นก่อนสตาร์ท modem) */
  (*(volatile uint32_t *)(SPU_BASE + 0x440)) = 0; /* EXTDOMAIN[0].PERM: SECATTR=0 */

  /* route IPC IRQ (42) ไปฝั่ง NS: NVIC_ITNS[1] bit10 (secure-only register) */
  (*(volatile uint32_t *)0xE000E384) |= (1u << (42 - 32));

  /* SAU ปิด + ALLNS=1: ให้ IDAU (SPU) เป็นผู้ตัดสิน attribution */
  SAU_CTRL = 0x2;
  __asm volatile("dsb; isb");

  /* ชี้ vector table ฝั่ง NS แล้วกระโดดแบบ BLXNS */
  const uint32_t *ns_vec = (const uint32_t *)NS_APP_BASE;
  SCB_NS_VTOR = NS_APP_BASE;
  __asm volatile("msr MSP_NS, %0" ::"r"(ns_vec[0]));
  uint32_t reset_ns = ns_vec[1] & ~1u; /* LSB=0 = ข้ามไป NS state */
  __asm volatile("blxns %0" ::"r"(reset_ns));
  for (;;) {
  }
}

void Default_Handler(void) {
  for (;;) {
  }
}

__attribute__((section(".vectors"), used)) static const uint32_t vectors[] = {
    0x20010000u, /* SP secure: ใช้ RAM ล่าง (region 0-7 ยัง secure) */
    (uint32_t)Reset_Handler,
    (uint32_t)Default_Handler,
    (uint32_t)Default_Handler,
};
