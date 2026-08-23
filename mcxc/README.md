# NXP MCX C starter (MCXC041 / FRDM-MCXC041)

Bare-metal template for NXP MCX C series (Kinetis KL03-class die):
Cortex-M0+, 32KB flash, 2KB SRAM. Flashes through the board's onboard
MCU-Link (CMSIS-DAP) with OpenOCD's `kinetis` driver — bundled with UnityMbed.

- `src/main.c` — your code. Starter cycles the FRDM RGB LED (PTB10/11/13).
- `linker.ld` — keeps the vector table AND the Flash Configuration Field
  (0x400, FSEC=0xFE). Do not remove the `.fcf` section: a wrong value there
  permanently locks the chip.
- Watchdog (COP) is disabled first thing in Reset_Handler — it resets the
  chip every ~1s otherwise.
