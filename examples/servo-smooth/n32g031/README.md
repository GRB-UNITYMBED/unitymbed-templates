# Smooth Servo Sweep (N32G031)

Sweeps a hobby servo smoothly between 0, 45 and 90 degrees and back, holding briefly at each stop.

> **Not tested on hardware yet.** This example compiles against the N32G031 base project, but nobody has run it on a board. Report problems on the templates repo.

## Pins

| Function | Pin | Notes |
| :-- | :-: | :-- |
| Servo signal | PA1 | Software (bit-banged) pulse |

## Wiring

| Servo wire (SG90 or similar) | Connect to |
| :-- | :-- |
| Brown / black (GND) | GND |
| Red (VCC) | 5V |
| Orange / yellow (signal) | PA1 |

Share ground between the servo supply and the board. A servo can draw more current than USB provides; use a separate 5V supply if the board resets while the servo moves.

## How it works

`src/main.c` generates each pulse in software with a busy-wait loop. The calibrated pulse constants are `PULSE_0_DEG` (2250), `PULSE_45_DEG` (3325) and `PULSE_90_DEG` (4400). `Smooth_Move()` steps between them 20 units at a time. Every servo is slightly different, so you may need to retune the constants; do not push them past the servo's mechanical limits.

## Build and flash

Create a new N32G031 project from this example in the UnityMbed IDE, then click **Build** and **Flash**.

Source: extracted from [GRB-UNITYMBED/N32G031_SERVO_SMOOTH](https://github.com/GRB-UNITYMBED/N32G031_SERVO_SMOOTH). Only the files that differ from the base project are stored here; the IDE writes them over [N32G031_BASE_PROJECT](https://github.com/GRB-UNITYMBED/N32G031_BASE_PROJECT).
