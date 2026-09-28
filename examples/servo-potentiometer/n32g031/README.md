# Servo Control with Potentiometer (N32G031)

Reads a potentiometer on the ADC and moves a hobby servo to match the knob position in real time. It also prints the raw and filtered readings over UART.

> **Not tested on hardware yet.** This example compiles against the N32G031 base project, but nobody has run it on a board. Report problems on the templates repo.

## Pins

| Function | Pin | Notes |
| :-- | :-: | :-- |
| Potentiometer wiper | PA0 | ADC channel 0 |
| Servo signal | PA1 | Software (bit-banged) pulse |
| UART TX | PA9 | USART1, 115200 8N1 |

## Wiring

| Device | Pin | Connect to |
| :-- | :-- | :-- |
| Potentiometer (10k–100k) | Wiper (middle) | PA0 |
| Potentiometer | One end | 3.3V |
| Potentiometer | Other end | GND |
| Servo (SG90 or similar) | Orange / yellow (signal) | PA1 |
| Servo | Red (VCC) | 5V |
| Servo | Brown / black (GND) | GND |

Share ground between the servo supply and the board. A servo can draw more current than USB provides; use a separate 5V supply if the board resets or the servo jitters while moving.

## How it works

`src/main.c` reads the ADC (0–4095), smooths it with a one-dimensional Kalman filter, then maps the value onto a pulse width between `PULSE_MIN` (2250, about 0°) and `PULSE_MAX` (5500, about 180°). Each pulse is generated in software with a busy-wait loop. Turning the knob left moves the servo toward 0°, turning it right moves it toward 180°. Every servo is slightly different, so you may need to retune the two constants; do not push them past the servo's mechanical limits.

Open the Serial Monitor at 115200 baud to see `Raw ADC`, `Filtered` and `Pulse` once every 50 loops.

## Build and flash

Create a new N32G031 project from this example in the UnityMbed IDE, then click **Build** and **Flash**.

Source: extracted from [GRB-UNITYMBED/N32G031_SERVO_POTENTIOMETER](https://github.com/GRB-UNITYMBED/N32G031_SERVO_POTENTIOMETER). Only the files that differ from the base project are stored here; the IDE writes them over [N32G031_BASE_PROJECT](https://github.com/GRB-UNITYMBED/N32G031_BASE_PROJECT).
