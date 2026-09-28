# OLED Temperature and Humidity (N32G031)

Reads a DHT11 sensor and shows temperature and humidity on a 128x64 I2C OLED (SSD1306-style). It also prints the readings over UART.

> **Not tested on hardware yet.** This example compiles against the N32G031 base project, but nobody has run it on a board. Report problems on the templates repo.

## Pins

| Function | Pin | Notes |
| :-- | :-: | :-- |
| DHT11 data | PA5 | Needs a pull-up if the module has none |
| OLED SCL | PB6 | Software I2C |
| OLED SDA | PB7 | Software I2C |
| UART TX | PA9 | USART1, 115200 8N1 (base `debug_uart`) |

PB6 and PB7 are also on-board LED pins, so those LEDs may flicker while the display updates.

## Wiring

| Device | Pin | Connect to |
| :-- | :-- | :-- |
| DHT11 | DATA | PA5 (add ~10k pull-up to VCC on a bare sensor) |
| DHT11 | VCC | 3.3V or 5V (check the module) |
| DHT11 | GND | GND |
| OLED (I2C) | SCL | PB6 |
| OLED (I2C) | SDA | PB7 |
| OLED (I2C) | VCC | 3.3V |
| OLED (I2C) | GND | GND |

## What you should see

The display shows "System Ready" for about 1.5 s, then the current temperature and humidity. It redraws only when a value changes. If the sensor does not answer, it shows "Sensor Error! Check Wiring."

The binary uses about 61 KB of the 64 KB flash (`printf`/`sprintf` plus the font table), which leaves little room for additions.

## Build and flash

Create a new N32G031 project from this example in the UnityMbed IDE, then click **Build** and **Flash**.

Source: extracted from [GRB-UNITYMBED/N32G031_OLED_DHT11](https://github.com/GRB-UNITYMBED/N32G031_OLED_DHT11). Only the files that differ from the base project are stored here; the IDE writes them over [N32G031_BASE_PROJECT](https://github.com/GRB-UNITYMBED/N32G031_BASE_PROJECT).
