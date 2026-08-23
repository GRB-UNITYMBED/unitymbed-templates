/*
 * nrf_modem_os glue — bare metal, single-thread + IPC ISR (ไม่มี RTOS)
 * สัญญาตาม include/nrf_modem_os.h ของ nrfxlib v2.5
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "nrf_errno.h"
#include "nrf_modem_os.h"

/* ── นาฬิกา: SysTick(NS) 1kHz + ตัวนับ ms ─────────────────────── */
#define SYST_CSR (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR (*(volatile uint32_t *)0xE000E018)
#define CPU_HZ 64000000u

static volatile uint32_t g_ms;
void SysTick_Handler(void) { g_ms++; }
uint32_t glue_ms(void) { return g_ms; }

void glue_clock_start(void) {
  SYST_RVR = (CPU_HZ / 1000u) - 1u;
  SYST_CVR = 0;
  SYST_CSR = 7; /* enable + IRQ + cpu clock */
}

void nrf_modem_os_busywait(int32_t usec) {
  /* นับจาก SysTick CVR (นับลง, reload = 1ms) — แม่นพอสำหรับ ~us */
  while (usec > 0) {
    uint32_t start = SYST_CVR;
    uint32_t ticks = (usec > 500) ? (CPU_HZ / 1000000u) * 500u : (CPU_HZ / 1000000u) * (uint32_t)usec;
    usec -= 500;
    while (((start - SYST_CVR) & 0xFFFFFFu) < ticks) {
      if (SYST_CVR > start) start = SYST_RVR; /* ข้าม reload */
    }
  }
}

int nrf_modem_os_sleep(uint32_t timeout) {
  uint32_t end = g_ms + timeout;
  while ((int32_t)(end - g_ms) > 0) __asm volatile("wfe");
  return 0;
}

/* ── heap แบบ free-list เรียบง่าย (2 instance: ทั่วไป + TX-shared) ── */
typedef struct blk {
  size_t size;
  struct blk *next;
} blk_t;
typedef struct {
  blk_t *free;
} heap_t;

static void heap_init(heap_t *h, void *base, size_t size) {
  blk_t *b = (blk_t *)base;
  b->size = size;
  b->next = NULL;
  h->free = b;
}
static void *heap_alloc(heap_t *h, size_t n) {
  n = (n + sizeof(blk_t) + 7u) & ~7u;
  for (blk_t **pp = &h->free; *pp; pp = &(*pp)->next) {
    blk_t *b = *pp;
    if (b->size >= n) {
      if (b->size >= n + 32) { /* แบ่งก้อน */
        blk_t *rest = (blk_t *)((uint8_t *)b + n);
        rest->size = b->size - n;
        rest->next = b->next;
        *pp = rest;
        b->size = n;
      } else {
        *pp = b->next;
      }
      return (uint8_t *)b + sizeof(blk_t);
    }
  }
  return NULL;
}
static void heap_free(heap_t *h, void *mem) {
  if (!mem) return;
  blk_t *b = (blk_t *)((uint8_t *)mem - sizeof(blk_t));
  b->next = h->free; /* ไม่ coalesce — พอสำหรับ pattern ของ lib */
  h->free = b;
}

static uint8_t g_heap_mem[16 * 1024] __attribute__((aligned(8)));
static heap_t g_heap, g_tx_heap;

/* shmem TX region — ต้องตรงกับที่ประกาศใน main.c */
extern uint8_t *const glue_tx_base;
extern const size_t glue_tx_size;

void *nrf_modem_os_alloc(size_t bytes) { return heap_alloc(&g_heap, bytes); }
void nrf_modem_os_free(void *mem) { heap_free(&g_heap, mem); }
void *nrf_modem_os_shm_tx_alloc(size_t bytes) { return heap_alloc(&g_tx_heap, bytes); }
void nrf_modem_os_shm_tx_free(void *mem) { heap_free(&g_tx_heap, mem); }

/* ── event / timedwait: ใช้ sequence counter + WFE ───────────── */
static volatile uint32_t g_evt_seq;

void nrf_modem_os_event_notify(uint32_t context) {
  (void)context;
  g_evt_seq++;
  __asm volatile("sev");
}

int32_t nrf_modem_os_timedwait(uint32_t context, int32_t *timeout) {
  (void)context;
  extern bool nrf_modem_is_initialized(void);
  if (!nrf_modem_is_initialized()) return -NRF_ESHUTDOWN;
  uint32_t seq0 = g_evt_seq;
  if (*timeout == 0) return -NRF_EAGAIN;
  uint32_t start = g_ms;
  for (;;) {
    if (g_evt_seq != seq0) {
      if (*timeout > 0) {
        uint32_t used = g_ms - start;
        *timeout = (used >= (uint32_t)*timeout) ? 0 : *timeout - (int32_t)used;
      }
      return 0;
    }
    if (*timeout > 0 && (g_ms - start) >= (uint32_t)*timeout) return -NRF_EAGAIN;
    __asm volatile("wfe");
  }
}

/* ── semaphore / mutex (pool เล็ก, single-thread + ISR) ──────── */
typedef struct {
  volatile int count;
  int limit;
  int used;
} sem_t_;
static sem_t_ g_sems[8];

int nrf_modem_os_sem_init(void **sem, unsigned int initial, unsigned int limit) {
  /* lib อาจเรียกซ้ำกับ sem เดิม — ต้อง re-init ไม่ใช่จองใหม่ */
  for (int i = 0; i < 8; i++) {
    if (*sem == &g_sems[i]) {
      g_sems[i].count = (int)initial;
      g_sems[i].limit = (int)limit;
      return 0;
    }
  }
  for (int i = 0; i < 8; i++) {
    if (!g_sems[i].used) {
      g_sems[i].used = 1;
      g_sems[i].count = (int)initial;
      g_sems[i].limit = (int)limit;
      *sem = &g_sems[i];
      return 0;
    }
  }
  return -NRF_ENOMEM;
}
void nrf_modem_os_sem_give(void *sem) {
  sem_t_ *s = sem;
  if (s->count < s->limit) s->count++;
  __asm volatile("sev");
}
int nrf_modem_os_sem_take(void *sem, int timeout) {
  sem_t_ *s = sem;
  uint32_t start = g_ms;
  for (;;) {
    __asm volatile("cpsid i");
    if (s->count > 0) {
      s->count--;
      __asm volatile("cpsie i");
      return 0;
    }
    __asm volatile("cpsie i");
    if (timeout == 0) return -NRF_EAGAIN;
    if (timeout > 0 && (g_ms - start) >= (uint32_t)timeout) return -NRF_EAGAIN;
    __asm volatile("wfe");
  }
}
unsigned int nrf_modem_os_sem_count_get(void *sem) { return (unsigned int)((sem_t_ *)sem)->count; }

static int g_mutex_slots[4];
int nrf_modem_os_mutex_init(void **mutex) {
  for (int i = 0; i < 4; i++) {
    if (*mutex == &g_mutex_slots[i]) { g_mutex_slots[i] = 0; return 0; }
  }
  for (int i = 0; i < 4; i++) {
    if (g_mutex_slots[i] != 2) { g_mutex_slots[i] = 2; *mutex = &g_mutex_slots[i]; return 0; }
  }
  return -NRF_ENOMEM;
}
int nrf_modem_os_mutex_lock(void *mutex, int timeout) {
  (void)timeout; /* single thread — ไม่มีการแย่ง */
  (void)mutex;
  return 0;
}
int nrf_modem_os_mutex_unlock(void *mutex) {
  (void)mutex;
  return 0;
}

/* ── เบ็ดเตล็ด ───────────────────────────────────────────────── */
static int g_errno;
void nrf_modem_os_errno_set(int e) { g_errno = e; }
int glue_errno(void) { return g_errno; }

bool nrf_modem_os_is_in_isr(void) {
  uint32_t ipsr;
  __asm volatile("mrs %0, ipsr" : "=r"(ipsr));
  return ipsr != 0;
}

void nrf_modem_os_init(void) {
  heap_init(&g_heap, g_heap_mem, sizeof(g_heap_mem));
  heap_init(&g_tx_heap, glue_tx_base, glue_tx_size);
}
void nrf_modem_os_shutdown(void) {}

void nrf_modem_os_log(int level, const char *fmt, ...) { (void)level; (void)fmt; }
void nrf_modem_os_logdump(int level, const char *str, const void *data, size_t len) {
  (void)level; (void)str; (void)data; (void)len;
}
