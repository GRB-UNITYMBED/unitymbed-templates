#ifndef NRFX_CONFIG_H
#define NRFX_CONFIG_H
#define NRFX_IPC_ENABLED 1
#define NRFX_IPC_DEFAULT_CONFIG_IRQ_PRIORITY 0
#endif
/* NS build: map alias เปล่าไปฝั่ง non-secure (ปกติ Zephyr เป็นคนเติมให้) */
#define NRF_IPC NRF_IPC_NS
