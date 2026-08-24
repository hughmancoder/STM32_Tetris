#include "display.h"
#include "stm32f4xx_hal.h"
#include "system.h"
#include <stdio.h>
#include "buttons.h"
#include "tetris_engine.h"

int main(void) {

  HAL_Init(); // Initialize the HAL Library (SysTick etc)

  usart2_init();
  MAX7219_SPI_Init();

  printf("\r\n=========================================\r\n");
  printf("  MAX7219 HARDWARE TEST SUITE STARTED    \r\n");
  printf("=========================================\r\n");

  // Essential startup configuration
  printf("[MAIN] Configuring MAX7219 essential registers...\r\n");
  max7219_init_all(0x09, 0x00); // Decode mode: No decode for all
  max7219_init_all(0x0A, 0x08); // Intensity: Medium brightness
  max7219_init_all(0x0B, 0x07); // Scan limit: Display all 8 rows
  max7219_init_all(0x0C, 0x01); // Shutdown: Wake up into normal operation
  
  // === DISPLAY TEST ===
  // Force all LEDs on for all matrices to debug power and data connections
  printf("[MAIN] Enabling Display Test Mode (All LEDs ON)...\r\n");
  max7219_init_all(0x0F, 0x01);

  // Clear all screens initially
  printf("[MAIN] Clearing all displays initially...\r\n");
  for (int row = 1; row <= 8; row++) {
    max7219_write_row(row, 0x00, 0x00, 0x00, 0x00);
  }

  printf("[MAIN] Initializing Buttons...\r\n");
  Buttons_Init();

  printf("[MAIN] Initializing Tetris Engine...\r\n");
  Tetris_Init();

  printf("[MAIN] Entering Main Game Loop.\r\n");

  uint32_t last_tick = HAL_GetTick();

  // Main Game Loop
  while (1) {
    uint32_t current_tick = HAL_GetTick();
    
    // Run the game loop at roughly 60Hz (16ms)
    if (current_tick - last_tick >= 16) {
      last_tick = current_tick;
      
      Buttons_Update();
      Tetris_Update();
      Tetris_Render();
    }
  }
}
