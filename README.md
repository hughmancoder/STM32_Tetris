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


## Pinout and Wiring

### 240x320 SPI Display

| Display Pin   | STM32 Pin     | GPIO Port   | Mode in CubeMX | Board Header / Pin      | Purpose / Description                                   |
| :------------ | :------------ | :---------- | :------------- | :---------------------- | :------------------------------------------------------ |
| **SCK / CLK** | PA5           | GPIOA       | SPI1_SCK (AF5) | Arduino D13 (CN5 pin 6) | Serial SPI clock line                                   |
| **MOSI / DIN**| PA7           | GPIOA       | SPI1_MOSI (AF5)| Arduino D11 (CN5 pin 4) | Serial data output 
| **CS**        | PB0           | GPIOB       | GPIO_Output    | Arduino A3 (CN8 pin 4)  | Chip Select (Active Low)                                |
| **DC / RS**   | PB1           | GPIOB       | GPIO_Output    | Morpho CN10 pin 24      | Data/Command selector line ($0 = \text{Cmd}, 1 = \text{Data}$) |
| **RESET / RST**| PB2          | GPIOB       | GPIO_Output    | Morpho CN10 pin 22      | Hardware display reset (Active Low)                     |
| **VCC**       | 3.3V (or 5V)  | Power Rail  | Power Header   | 3V3 / 5V                | Display logic & panel power                             |
| **GND**       | GND           | Power Rail  | Ground Header  | GND                     | Common ground                                           |
| **LED / BLK** | 3.3V          | Power Rail  | Power Header   | 3V3                     | Display backlight supply                                |

### Push Buttons

> **Wiring**: For a bare 2-pin push button on a breadboard, connect one pin to the STM32 GPIO and the other pin to **GND**. Pins are configured in CubeMX as **Active-Low** with internal **Pull-up** resistors (`GPIO_PULLUP`).

| Button Name    | STM32 Pin | GPIO Port | Mode in CubeMX       | Board Header / Pin      | Game Action      |
| :------------- | :-------- | :-------- | :------------------- | :---------------------- | :--------------- |
| **Btn_Left**   | PC0       | GPIOC     | GPIO_Input (Pull-up) | Arduino A5 (CN8 pin 6)  | Move Left        |
| **Btn_Right**  | PC1       | GPIOC     | GPIO_Input (Pull-up) | Arduino A4 (CN8 pin 5)  | Move Right       |
| **Btn_Rotate** | PC2       | GPIOC     | GPIO_Input (Pull-up) | Morpho CN7 pin 35       | Rotate Piece     |
| **Btn_Drop**   | PC3       | GPIOC     | GPIO_Input (Pull-up) | Morpho CN7 pin 37       | Hard / Fast Drop |