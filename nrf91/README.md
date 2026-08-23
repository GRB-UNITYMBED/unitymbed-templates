# nRF9160 standalone — bare-metal LTE-M/NB-IoT (NO Zephyr, NO TF-M)

UnityMbed template for the nRF9160 (agri-sensor class devices). Talks to the
LTE modem from plain C — no RTOS, no SDK, ~60KB total.

## Layout

| Path | What it is | Touch it? |
|---|---|---|
| `src/main.c` | **Your application** (non-secure side). Starter: modem init → `AT+CGMR` → network registration, status on LED1-4 | yes |
| `sys/glue.c` | `nrf_modem_os_*` glue (heaps, semaphores, SysTick) required by libmodem | rarely |
| `secure/` | Mini secure boot (~900B): mdk `SystemInit()` + SPU config, then jumps to NS app at 0x40000. Replaces TF-M | no |
| `lib/` | Vendored deps: `libmodem.a` (Nordic nrfxlib v2.3.0), `nrfx_ipc.o`, mdk + CMSIS headers | no |
| `linker.ld` | NS app: flash 0x40000+, RAM 0x20018000+ (below is modem shared memory) | sizes only |
| `flash.jlink` | J-Link script: flashes both images. `unitymbed flash` uses the J-Link backend (DK onboard probe) | no |

## Hard-won rules (violating any = modem silently dead)

1. `SystemInit()` must run in the secure boot **before anything else** — undocumented modem dependency.
2. Delays: use `nrf_modem_os_busywait()`, NOT a SysTick-IRQ counter (drifts ~100x slow after modem init).
3. `+CEREG:` replies: first number is the report *mode* — registered means `,1` (home) or `,5` (roaming) in the *second* field.
4. Shared memory map is fixed: ctrl 0x20010000, TX 0x20012000, RX 0x20014000. NS app RAM starts at 0x20018000.
5. The IPC vector must forward to `nrfx_ipc_irq_handler` (already wired in `src/main.c`).

## Requirements

- SIM with LTE-M/NB-IoT service (Thailand: AIS IoT SIM). Foreign roaming SIMs typically get rejected at attach.
- SEGGER J-Link software installed (`JLinkExe`) — the DK's onboard probe uses it.

LEDs (nRF9160-DK): LED1 = modem init OK · LED2 = AT OK · LED3 = network registered · LED4 blink = searching / fast blink = init fail.
