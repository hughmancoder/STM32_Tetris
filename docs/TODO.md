Phase 1: Environment & Project Structure
- [ ] Initialise the Build Environment

- [ ] Install dependencies via Homebrew (arm-none-eabi-gcc, openocd, cmake, ninja).

- [ ] Set up the directory structure (src, inc, build).

- [ ] Configure the Microcontroller

- [ ] Generate startup code and linker scripts (can use STM32CubeMX strictly to generate the CMSIS headers and startup assembly, disabling the HAL if you prefer pure bare-metal register access).

- [ ] Write a basic main.c that toggles the onboard LED (PA5) to verify the build and flash process.

Phase 2: Hardware Initialisation
- [ ] System Clock Configuration

- [ ] Configure the RCC (Reset and Clock Control) registers to set the PLL and drive the system clock to 180 MHz.

- [ ] Peripheral Clocks

- [ ] Enable clock gating for GPIO ports (e.g., GPIOA, GPIOB) and SPI peripherals.

- [ ] Setup Hardware Timer (Game Tick)

- [ ] Initialise a general-purpose timer (e.g., TIM2) to generate interrupts for the primary game loop/gravity tick.

Phase 3: Display Driver (SPI)
- [ ] SPI Initialisation

- [ ] Configure GPIO pins for SPI (SCK, MOSI, CS, DC, RESET).

- [ ] Set the SPI baud rate and frame format (8-bit or 16-bit).

- [ ] Display Initialisation Sequence

- [ ] Write the specific command sequence required to wake up and configure the display controller (e.g., ILI9341).

- [ ] Graphics Primitives

- [ ] Write functions to set the drawing window and push pixel colour data.

- [ ] Implement draw_rectangle() and fill_screen() to handle block rendering.

- [ ] Optimisation (Optional but recommended)

- [ ] Configure DMA (Direct Memory Access) to push the frame buffer to the SPI peripheral without blocking the CPU.

Phase 4: Inputs & Game Logic
- [ ] Input Handling

- [ ] Configure GPIO pins as inputs with internal pull-up resistors for directional buttons (Left, Right, Rotate, Hard Drop).

- [ ] Implement hardware debouncing (using a timer) or software debouncing logic.

- [ ] Game State & Data Structures

- [ ] Define the 10x20 playfield as a 2D array or bitboard.

- [ ] Encode the 7 Tetromino shapes (I, J, L, O, S, T, Z) and their rotation states into constant arrays.

- [ ] Core Mechanics

- [ ] Implement collision detection (wall kicks, floor, and stacked blocks).

- [ ] Implement rotation logic.

- [ ] Implement line clearing and score calculation.

Phase 5: Audio & Polish (Advanced)
- [ ] Audio Generation

- [ ] Utilise a DAC or configure a timer for PWM audio to play the Tetris theme.

- [ ] Implement basic digital signal processing or tone generation logic to drive a small speaker.

- [ ] Game Flow

- [ ] Build a simple state machine (Start Screen -> Playing -> Game Over).

- [ ] Add a level progression system that increases the timer interrupt frequency (speeding up the game).
"""