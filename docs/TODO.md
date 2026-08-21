Phase 0: Environment & Project Structure
- [x] Initialise the Build Environment
- [x] Set up the directory structure (src, inc, build).
- [x] Configure the Microcontroller
- [x] Generate startup code and linker scripts.
- [x] Write a basic main.c that toggles the onboard LED (PA5) to verify the build and flash process.

Phase 1: Working Baseline with HAL (1–2 weeks)
- [ ] Use STM32CubeMX to generate the clock configuration (180 MHz), GPIOs, and basic SPI.
- [ ] Initialize the display using standard HAL functions (HAL_SPI_Transmit).
- [ ] Implement the core Tetris game logic (array matrices, collision detection, game ticks).

Phase 2: Add Professional Scaffolding (1 week)
- [ ] Implement Off-Target Unit Tests for game mechanics (line clears, rotations) using Unity or GoogleTest on your host machine.
- [ ] Showcase clean modular design by isolating platform-independent game logic from hardware drivers.

Phase 3: The Bare-Metal Optimization (Resume Polish)
- [ ] Replace HAL_SPI_Transmit with direct register writes (SPI->DR, SPI->SR) and configure DMA streams directly via registers.
- [ ] Add a dedicated section to your project's README.md documenting this migration (e.g., "Replaced vendor HAL SPI driver with bare-metal CMSIS registers + DMA, increasing frame rendering throughput").
"""