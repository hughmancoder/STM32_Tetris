#ifndef TETRIS_ENGINE_H
#define TETRIS_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

#define TETRIS_ROWS 32
#define TETRIS_COLS 8

// The main game state, representing the 32x8 grid
extern uint8_t tetris_grid[TETRIS_ROWS];

// Initialize the game state
void Tetris_Init(void);

// Update the game state based on inputs (called once per tick)
void Tetris_Update(void);

// Render the current game state to the MAX7219 displays
void Tetris_Render(void);

#endif // TETRIS_ENGINE_H
