# Infineon XMC1100 starter (XMC 2Go / Boot Kit)

Bare-metal template for the XMC1100 (Cortex-M0, 64KB flash, 16KB SRAM).
Flashes through the board's XMC-Link (J-Link OB) with OpenOCD's `xmc1xxx`
driver — bundled with UnityMbed.

Hard-won rules (measured on silicon):
- User flash lives at **0x10001000**, not 0x0 — `linker.ld` handles it.
- Vector slots 0x10/0x14 are **CLK_VAL1/CLK_VAL2** read by the Boot ROM;
  the template keeps them 0xFFFFFFFF (default 8 MHz clocks). Never put a
  handler there.
- The board has no SRST line — flashing uses SYSRESETREQ (`openocd.cfg`).
- GPIO OMR: bit n sets Pn, bit n+16 resets it, both at once = toggle.
