# LED Sequence (N32G031)

Lights the on-board LEDs one at a time, PB1 then PB3, PB6 and PB7, to make a chasing pattern.

> **Not tested on hardware yet.** This example compiles against the N32G031 base project, but nobody has run it on a board. Report problems on the templates repo.

## Pins

| Function | Pin | Notes |
| :-- | :-: | :-- |
| On-board LED 1 | PB1 | Already wired on the board |
| On-board LED 2 | PB3 | Already wired on the board |
| On-board LED 3 | PB6 | Already wired on the board |
| On-board LED 4 | PB7 | Already wired on the board |

No extra hardware is needed.

## How it works

`src/main.c` configures the four pins as push-pull outputs and steps through them with a busy-wait `delay(400000)`. Lower the number to make the chase faster.

## Build and flash

Create a new N32G031 project from this example in the UnityMbed IDE, then click **Build** and **Flash**.

Source: extracted from [GRB-UNITYMBED/N32G031_LED_SEQUENCE](https://github.com/GRB-UNITYMBED/N32G031_LED_SEQUENCE). Only the files that differ from the base project are stored here; the IDE writes them over [N32G031_BASE_PROJECT](https://github.com/GRB-UNITYMBED/N32G031_BASE_PROJECT).
