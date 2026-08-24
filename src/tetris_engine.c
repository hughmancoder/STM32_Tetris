#include "tetris_engine.h"
#include "display.h"
#include "buttons.h"
#include <string.h>

uint8_t tetris_grid[TETRIS_ROWS];

// For the initial test, we will just track a single pixel
static int player_x = 3;
static int player_y = 15;

void Tetris_Init(void) {
  memset(tetris_grid, 0, sizeof(tetris_grid));
  player_x = 3;
  player_y = 15;
}

void Tetris_Update(void) {
  // Clear the grid
  memset(tetris_grid, 0, sizeof(tetris_grid));

  // Handle inputs
  if (Button_WasPressed(BTN_LEFT)) {
    player_x++;
    if (player_x >= TETRIS_COLS) player_x = TETRIS_COLS - 1;
  }
  if (Button_WasPressed(BTN_RIGHT)) {
    player_x--;
    if (player_x < 0) player_x = 0;
  }
  if (Button_WasPressed(BTN_ROTATE)) {
    // For now, let's use the rotate button to move down
    player_y--;
    if (player_y < 0) player_y = 0;
  }

  // Draw the player pixel on the grid
  tetris_grid[player_y] |= (1 << player_x);
}

void Tetris_Render(void) {
  // The MAX7219 matrices are daisy-chained. 
  // Matrix 0 controls rows 1-8
  // Matrix 1 controls rows 9-16
  // Matrix 2 controls rows 17-24
  // Matrix 3 controls rows 25-32

  for (int row = 1; row <= 8; row++) {
    uint8_t d0 = tetris_grid[row - 1];
    uint8_t d1 = tetris_grid[row + 7];
    uint8_t d2 = tetris_grid[row + 15];
    uint8_t d3 = tetris_grid[row + 23];
    
    max7219_write_row(row, d3, d2, d1, d0);
  }
}
