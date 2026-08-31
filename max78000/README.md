# MAX78000 — bare-metal starter

Analog Devices (formerly Maxim) MAX78000, **Cortex-M4 core only**.

The CNN accelerator that is the reason this part exists is not covered here.
Using it needs Maxim's own `ai8x-training` / `ai8x-synthesis` toolchain to
turn a trained model into C, which is a separate pipeline from this template.

## Blink

`src/main.c` toggles the green user LED on a MAX78000FTHR. Two addresses on
this part surprise people who assume it looks like an STM32:

| | |
|---|---|
| Flash | `0x10000000` — not `0` and not `0x08000000` |
| GPIO2 | `0x40080400` — it does **not** continue the GPIO0/GPIO1 spacing |
| GPIO2 clock | `LPGCR` at `0x40080000`, not the usual `GCR` |

GPIO2 sits in the low-power domain, which is why both its address and its
clock gate live somewhere else. Extrapolating `0x4000A000` from GPIO0 and
GPIO1 builds and flashes cleanly and leaves the LED dark.

Every constant in `main.c` is transcribed from the vendor headers
(`analogdevicesinc/msdk`), with the source named in a comment beside it.

## Build and flash

```
unitymbed build
unitymbed flash
```

`openocd.cfg` builds the DAP and the flash bank by hand — OpenOCD ships no
target config for this part. The `max32xxx` flash driver it uses is compiled
into the OpenOCD UnityMbed bundles.

## Pins

LEDs are GPIO2 pins 0 (red), 1 (green), 2 (blue), and they are **active low**.
