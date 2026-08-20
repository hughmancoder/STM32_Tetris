# STM32 Build Process & PlatformIO Guide

This document provides a brief rundown of how your C code goes from text on your screen to a running program on the STM32 microcontroller.

## 1. The Build Process
When you click **Upload** or run `pio run`, PlatformIO performs several steps in the background:

### A. Preprocessing & Compilation
- **Preprocessor:** The GCC ARM compiler first looks at all your `.c` files. It processes any directives like `#include "stm32f4xx_hal.h"` by literally copying the contents of those header files into your source code behind the scenes.
- **Compilation:** The compiler then translates your human-readable C code into Assembly language, and finally into **Object files** (`.o`). These contain raw machine code for the ARM Cortex-M4 processor, but they don't know where in the memory they will live yet.

### B. Library Linking
- If you use functions like `HAL_Delay()` or `HAL_GPIO_WritePin()`, the compiler needs to know where those functions are. 
- PlatformIO automatically downloads the **STM32Cube HAL (Hardware Abstraction Layer)** framework for your specific board. It compiles the required HAL files into a library archive.

### C. Linking
- The **Linker** takes all your object files (like `main.o` and `sos.o`) and merges them with the HAL libraries.
- It uses a **Linker Script** (`.ld` file) specific to the STM32F446RE. This script tells the linker exactly how much Flash memory (512KB) and RAM (128KB) the chip has, and where to map the code into memory (usually starting at address `0x08000000`).
- The output of the linker is the final **ELF binary executable** (`firmware.elf`).

### D. Flashing (Uploading)
- PlatformIO uses a tool called **OpenOCD** to talk to the built-in ST-Link programmer on your Nucleo board via USB.
- OpenOCD halts the processor, writes the `firmware.elf` binary directly into the Flash memory of the STM32, verifies the data, and then resets the chip to start executing your code!

## 2. Linking Multiple Files
In C, you can split your code into multiple files to keep it organized (for example, putting our SOS logic in a separate file).

1. **Header File (`.h`):** Acts as the "table of contents". It contains the *declaration* of the function so that `main.c` knows it exists.
2. **Source File (`.c`):** Contains the actual *implementation* (the logic) of the function.

When you include the header (`#include "sos.h"`) in `main.c`, the compiler knows what `play_sos()` looks like. Later, the **Linker** (Step C) connects the call in `main.c` to the actual compiled code from `sos.c`. PlatformIO automatically finds all `.c` and `.cpp` files in the `src/` directory and compiles/links them together without you needing to write a complex Make script!
