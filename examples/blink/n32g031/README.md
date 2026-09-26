# Blink LED (N32G031)

Toggles the on-board LED on **PB7** every 500 ms. The smallest possible GPIO program.

> **Not tested on hardware yet.** This example compiles against the N32G031 base project, but nobody has run it on a board. Report problems on the templates repo.

## Pins

| Function | Pin | Notes |
| :-- | :-: | :-- |
| On-board LED | PB7 | Already wired on the board |

No extra hardware is needed.

## How it works

`src/main.c` enables the GPIOB clock, sets PB7 to push-pull output by writing the registers directly, and flips the output bit in a loop. The delay comes from `delay_ms()` in the base project's `src/utils.c` (SysTick based).

## Build and flash

Create a new N32G031 project from this example in the UnityMbed IDE, then click **Build** and **Flash**.

Source: extracted from [GRB-UNITYMBED/N32G031_Blinky](https://github.com/GRB-UNITYMBED/N32G031_Blinky). Only the files that differ from the base project are stored here; the IDE writes them over [N32G031_BASE_PROJECT](https://github.com/GRB-UNITYMBED/N32G031_BASE_PROJECT).
