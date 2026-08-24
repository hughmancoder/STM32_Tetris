# STM32 Tetris

Built for STM32 Nucleo-F446RE

## Hardware

- 2x1 buttons
- max7128 display

## Getting Started

This project is configured to build using [PlatformIO](https://platformio.org/).

### Prerequisites

- Install the **PlatformIO IDE** extension in VS Code (Search for `PlatformIO IDE` in the Extensions marketplace).

### How to Build and Flash (CLI)

Refer to makefile 

- **Build Only:** `pio run`
- **Build & Upload:** `pio run -t upload`
- **Clean Project:** `pio run -t clean`
- **Monitor Serial:** `pio device monitor`



| MAX7219 Pin | Nucleo Pin | Function |
| :--- | :--- | :--- |
| **VCC** | 5V | Power supply |
| **GND** | GND | Ground connection |
| **DIN** | PA7 (D11) | SPI1 MOSI (Data In) |
| **CLK** | PA5 (D13) | SPI1 SCK (Clock) |
| **CS** | PA0 (A0) | Chip Select (Standard Output) |