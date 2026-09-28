# 3-Channel 20 kHz PWM (N32G031)

Reads a potentiometer on PA0 and drives three 20 kHz hardware PWM outputs with the matching duty cycle. Prints the ADC value and duty cycle over UART and keeps a window watchdog fed.

> **Not tested on hardware yet.** This example compiles against the N32G031 base project, but nobody has run it on a board. Report problems on the templates repo.

## Pins

| Function | Pin | Notes |
| :-- | :-: | :-- |
| Potentiometer wiper | PA0 | ADC channel 0 |
| PWM out (scope / driver) | PA8 | TIM1 CH1, 20 kHz |
| PWM out 1 | PB0 | TIM3 CH3, 20 kHz |
| PWM out 2 | PB1 | TIM3 CH4, 20 kHz |
| UART TX | PA9 | USART1, 115200 8N1 |

## Wiring

| Device | Connect |
| :-- | :-- |
| 10k potentiometer | Ends to 3.3V and GND, wiper to PA0 |
| LED + resistor (optional) | PB0 and/or PB1 to GND through ~330 ohm |
| Oscilloscope (optional) | Probe on PA8, ground clip on GND |
| USB-serial adapter (optional) | Adapter RX to PA9, GND to GND |

Turning the knob changes the duty cycle from 0% to 100% on all three outputs.

## How it works

`src/main.c` configures the ADC, TIM1 and TIM3 through the Nations standard peripheral library that ships in the base project's `drivers/` folder. The window watchdog is set up with direct register writes; if the main loop stalls, it resets the MCU, so continuous resets usually mean the loop is blocked.

The binary uses about 52 KB of the 64 KB flash, mostly because of `printf`.

## Build and flash

Create a new N32G031 project from this example in the UnityMbed IDE, then click **Build** and **Flash**.

Source: extracted from [GRB-UNITYMBED/N32G031_3CH_PWM_20kHz](https://github.com/GRB-UNITYMBED/N32G031_3CH_PWM_20kHz). Only the files that differ from the base project are stored here; the IDE writes them over [N32G031_BASE_PROJECT](https://github.com/GRB-UNITYMBED/N32G031_BASE_PROJECT).
