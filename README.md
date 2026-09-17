# STM32 Tetris

Built for STM32 Nucleo-F446RE

## Hardware

- 2x1 buttons
- 240X320 SPI Display

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


## Buttons

PC0-3

For a bare 2-pin push button on a breadboard,  configure the pins as Active-Low with a Pull-up resistor.


## Ports

| Display Pin | STM32 Pin | GPIO Port | Mode in CubeMX | Arduino Header | Purpose / Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| SCK / CLK | PA5 | GPIOA | SPI1_SCK (AF5) | D13 (CN5 pin 6) | Serial SPI clock line |
| MOSI / DIN | PA7 | GPIOA | SPI1_MOSI (AF5) | D11 (CN5 pin 4) | Serial data output (pixels & commands) |
| CS | PB0 | GPIOB | GPIO_Output | A3 (CN8 pin 4) | Chip Select (Active Low) |
| DC / RS | PB1 | GPIOB | GPIO_Output | — (CN10 pin 24) | Data/Command selector line ($0 = \text{Cmd}$, $1 = \text{Data}$) |
| RESET / RST | PB2 | GPIOB | GPIO_Output | — (CN10 pin 22) | Hardware display reset |
| VCC | 3.3V (or 5V) | Power Rail | Power Header | 3V3 / 5V | Display logic & panel power |
| GND | GND | Power Rail | Ground Header | GND | Common ground |
| LED / BLK | 3.3V | Power Rail | Power Header | 3V3 | Display backlight supply |
| PC0 | Btn_Left | GPIO_Input  Pull-up | A5 (CN8 Pin 6) | Move Left |
| PC1 | Btn_Right | GPIO_Input  Pull-up | A4 (CN8 Pin 5) | Move Right |
| PC2 | Btn_Rotate | GPIO_Input  Pull-up | — | Rotate Piece |
| PC3 | Btn_Drop | GPIO_Input  Pull-up | — | Hard / Fast Drop |