# N32G452 Blank

Cortex-M4F, 256KB flash, 48KB RAM. Empty `main()` — no clocks, GPIO or peripherals configured.
Same startup, linker script, Makefile and OpenOCD config as the `n32g452` (Blinky) template.

```bash
unitymbed build
unitymbed flash
```
