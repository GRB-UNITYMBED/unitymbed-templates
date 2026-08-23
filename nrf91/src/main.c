/*
 * NS app Step 2 — จุด nrf_modem_init + ยิง AT+CGMR จากโค้ด bare-metal เราเอง
 *
 * รายงานผลทาง LED (nRF9160-DK P0.02..P0.05):
 *   LED1 ติดค้าง            = ผ่าน nrf_modem_init แล้ว
 *   LED1+LED2 ติดค้าง       = AT+CGMR ตอบสำเร็จ (เส้นชัย Step 2!)
 *   LED4 กระพริบเร็ว         = init fail (อ่านโค้ด error ใน g_result ผ่าน debugger)
 * ตรวจละเอียดผ่าน debugger: g_result / g_at_resp ใน RAM
 */
#include <stdint.h>
#include <string.h>

#include "nrf_modem.h"
#include "nrf_modem_at.h"

#define GPIO_NS 0x40842500u
#define OUTSET (*(volatile uint32_t *)(GPIO_NS + 0x008))
#define OUTCLR (*(volatile uint32_t *)(GPIO_NS + 0x00C))
#define DIRSET (*(volatile uint32_t *)(GPIO_NS + 0x018))
/* LED pins ต่างกันตามบอร์ด — Makefile เลือก define ตาม SOC=
 *   nRF9160-DK: LED1-4 = P0.02-05
 *   nRF9151-DK: LED1-4 = P0.00/01/04/05 */
#ifdef UM_BOARD_NRF9151DK
#define LED1 (1u << 0)
#define LED2 (1u << 1)
#define LED3 (1u << 4)
#define LED4 (1u << 5)
#else
#define LED1 (1u << 2)
#define LED2 (1u << 3)
#define LED3 (1u << 4)
#define LED4 (1u << 5)
#endif

/* ── shared memory ให้ modem: ต้องอยู่ RAM ฝั่ง NS ── */
#define SHM_BASE 0x20010000u
#define SHM_CTRL SHM_BASE                     /* 0x4E8 */
#define SHM_TX (SHM_BASE + 0x2000u)           /* 8KB */
#define SHM_RX (SHM_BASE + 0x4000u)           /* 8KB */
#define SHM_TXRX_SIZE 0x2000u

uint8_t *const glue_tx_base = (uint8_t *)SHM_TX;
const size_t glue_tx_size = SHM_TXRX_SIZE;

extern void glue_clock_start(void);
extern uint32_t glue_ms(void);
extern void nrfx_ipc_irq_handler(void); /* ISR ของ nrfx — dispatch ต่อให้ libmodem เอง */

volatile int g_result = -999;      /* debugger อ่านตรงนี้ */
char g_at_resp[128];
char g_cereg[96];                  /* สถานะเครือข่ายล่าสุด */
volatile unsigned int g_polls = 0;

void IPC_IRQHandler(void) { nrfx_ipc_irq_handler(); }

static void fault_handler(struct nrf_modem_fault_info *info) {
  (void)info;
  g_result = -1000;
}

/* v2.5.3 บังคับต้องมี dfu_handler — ไม่ใส่ = init ตอบ -EINVAL เงียบๆ
 * (เราไม่ทำ modem DFU ใน template นี้ แค่รับ result ไว้ดูผ่าน debugger) */
static volatile uint32_t g_dfu_result;
static void dfu_handler(uint32_t dfu_result) { g_dfu_result = dfu_result; }


extern void nrf_modem_os_busywait(int32_t usec);
static void delay_ms(uint32_t ms) { nrf_modem_os_busywait((int32_t)ms * 1000); }

/* NVIC ฝั่ง NS */
#define NVIC_ISER1 (*(volatile uint32_t *)0xE000E104)
#define NVIC_IPR42 (*(volatile uint8_t *)(0xE000E400 + 42))

int main(void) {
  DIRSET = LED1 | LED2 | LED4;
  OUTCLR = LED1 | LED2 | LED4;
  glue_clock_start();

  NVIC_IPR42 = (1u << 5); /* prio 1 — ให้ SysTick (prio 0) แซงได้ กัน busywait-ใน-ISR แย่งเวลา */
  NVIC_ISER1 = (1u << (42 - 32)); /* enable IPC IRQ */

  const struct nrf_modem_init_params params = {
      .shmem = {
          .ctrl = { .base = SHM_CTRL, .size = NRF_MODEM_SHMEM_CTRL_SIZE },
          .tx = { .base = SHM_TX, .size = SHM_TXRX_SIZE },
          .rx = { .base = SHM_RX, .size = SHM_TXRX_SIZE },
      },
      .ipc_irq_prio = 1,
      .fault_handler = fault_handler,
      .dfu_handler = dfu_handler,
  };

  g_result = nrf_modem_init(&params);
  if (g_result != 0) {
    for (;;) { /* LED4 กระพริบเร็ว = init fail */
      OUTSET = LED4; delay_ms(120); OUTCLR = LED4; delay_ms(120);
    }
  }
  OUTSET = LED1; /* init ผ่าน! */

  int r = nrf_modem_at_cmd(g_at_resp, sizeof(g_at_resp), "AT+CGMR");
  if (r == 0) {
    OUTSET = LED2; /* Step 2 ✓ */
  } else {
    g_result = r;
    for (;;) __asm volatile("wfe");
  }

  /* ── Step 3: เปิดวิทยุ + รอ register เครือข่ายไทย ── */
  nrf_modem_at_cmd(g_cereg, sizeof(g_cereg), "AT%%XSYSTEMMODE=1,1,0,0");
  nrf_modem_at_cmd(g_cereg, sizeof(g_cereg), "AT+CEREG=5"); /* รายงาน reject cause */
  r = nrf_modem_at_cmd(g_cereg, sizeof(g_cereg), "AT+CFUN=1");
  if (r != 0) { g_result = 300 + (r < 0 ? -r : r); for (;;) __asm volatile("wfe"); }

  for (;;) { /* poll ทุก 2 วิ — LED4 กะพริบระหว่างหา, LED3 ติดค้างเมื่อ registered */
    delay_ms(2000);
    g_polls++;
    r = nrf_modem_at_cmd(g_cereg, sizeof(g_cereg), "AT+CEREG?");
    if (r == 0 && (strstr(g_cereg, ",1") || strstr(g_cereg, ",5"))) {
      OUTCLR = LED4;
      OUTSET = LED3; /* REGISTERED! เส้นชัย Step 3 */
      break;
    }
    OUTSET = LED4; delay_ms(80); OUTCLR = LED4;
    if (g_polls % 10u == 0u) nrf_modem_at_cmd(g_at_resp, sizeof(g_at_resp), "AT+CEER"); /* สาเหตุล่าสุด */
  }
  for (;;) __asm volatile("wfe");
}

/* ── startup: copy .data + zero .bss แล้วเข้า main ── */
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss;
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
void SysTick_Handler(void);

#define DH (uint32_t) Default_Handler
__attribute__((section(".vectors"), used)) static const uint32_t vectors[16 + 64] = {
    0x20030000u, (uint32_t)Reset_Handler, DH, DH, DH, DH, DH, 0, 0, 0, 0, DH, DH, 0,
    DH,                        /* PendSV */
    (uint32_t)SysTick_Handler, /* SysTick */
    /* external IRQs 0..63 — ทุกช่องชี้ Default ยกเว้น IPC(42) */
    [16 + 42] = (uint32_t)IPC_IRQHandler,
    [16 + 63] = DH,
};
