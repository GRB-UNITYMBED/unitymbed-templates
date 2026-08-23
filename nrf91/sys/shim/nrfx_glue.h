#ifndef NRFX_GLUE_H
#define NRFX_GLUE_H
#include <nrfx.h>
#define NRFX_ASSERT(x) do { if (!(x)) { for(;;){} } } while (0)
#define NRFX_STATIC_ASSERT(x)
#define NRFX_IRQ_PRIORITY_SET(irq, pri) (*(volatile uint8_t *)(0xE000E400u + (uint32_t)(irq)) = (uint8_t)((pri) << 5))
#define NRFX_IRQ_ENABLE(irq)  (*(volatile uint32_t *)(0xE000E100u + 4u*((uint32_t)(irq)/32u)) = 1u << ((uint32_t)(irq)%32u))
#define NRFX_IRQ_DISABLE(irq) (*(volatile uint32_t *)(0xE000E180u + 4u*((uint32_t)(irq)/32u)) = 1u << ((uint32_t)(irq)%32u))
#define NRFX_IRQ_IS_ENABLED(irq) ((*(volatile uint32_t *)(0xE000E100u + 4u*((uint32_t)(irq)/32u)) >> ((uint32_t)(irq)%32u)) & 1u)
#define NRFX_IRQ_PENDING_SET(irq) (*(volatile uint32_t *)(0xE000E200u + 4u*((uint32_t)(irq)/32u)) = 1u << ((uint32_t)(irq)%32u))
#define NRFX_IRQ_PENDING_CLEAR(irq) (*(volatile uint32_t *)(0xE000E280u + 4u*((uint32_t)(irq)/32u)) = 1u << ((uint32_t)(irq)%32u))
#define NRFX_CRITICAL_SECTION_ENTER() __asm volatile("cpsid i")
#define NRFX_CRITICAL_SECTION_EXIT()  __asm volatile("cpsie i")
#define NRFX_DELAY_US(us) nrf_modem_os_busywait((int32_t)(us))
void nrf_modem_os_busywait(int32_t usec);
#endif
