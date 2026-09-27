# N32G455 Blank

Cortex-M4F, 512KB flash, 144KB RAM. Empty `main()` — no clocks, GPIO or peripherals configured.
Same startup, linker script, Makefile and OpenOCD config as the `n32g455` (Blinky) template.

```bash
unitymbed build
unitymbed flash
```
