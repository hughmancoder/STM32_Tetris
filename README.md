# STM32 Tetris

Nucleo STM32F446

## Getting Started

This project is configured to build using [PlatformIO](https://platformio.org/).

### Prerequisites
1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install the **PlatformIO IDE** extension in VS Code (Search for `PlatformIO IDE` in the Extensions marketplace).

### How to Build and Flash (VS Code)
1. Connect your **Nucleo-F446RE** board to your Mac via the USB port on the ST-Link (the top section of the board).
2. Open this project folder (`tetris-stm-32`) in VS Code.
3. To compile the code and flash it to your board, click the **Upload** button (the right arrow `➔` icon) located in the bottom status bar of VS Code.
   - *Alternatively, you can click the Alien logo on the left sidebar, go to `Project Tasks -> nucleo_f446re -> General`, and click `Upload`.*
4. Once the flashing process completes successfully, the green user LED (LD2) on the board will start blinking twice a second!

### How to Build and Flash (Terminal / CLI)
If you don't see the buttons in VS Code, you can use the PlatformIO CLI directly from your terminal. Open the terminal inside this folder and run:

- **Build Only:** `pio run`
- **Build & Upload:** `pio run -t upload`
- **Clean Project:** `pio run -t clean`
- **Monitor Serial:** `pio device monitor`

#### Making `pio` available globally in your terminal:
If `pio` gives `command not found`, add it to your `~/.zshrc` by running:
```bash
echo 'export PATH="$HOME/.platformio/penv/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```