/**
 * @file display.h
 * @brief High-performance SPI TFT display driver (ILI9341) and Tetris renderer.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "main.h"
#include "spi.h"
#include "tetris_core.h"
#include "tetromino.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  320

// Playfield dimensions (10 cols x 20 rows, 16x16 px per block)
#define BLOCK_SIZE      16
#define PLAYFIELD_X     0
#define PLAYFIELD_Y     0
#define PLAYFIELD_W     (TETRIS_BOARD_COLS * BLOCK_SIZE)  // 160 px
#define PLAYFIELD_H     (TETRIS_BOARD_ROWS * BLOCK_SIZE)  // 320 px

// HUD Panel dimensions
#define HUD_X           PLAYFIELD_W                       // 160 px
#define HUD_W           (DISPLAY_WIDTH - PLAYFIELD_W)     // 80 px

// Color definitions (RGB565)
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_NAVY      0x000F
#define COLOR_DARKBLUE  0x0112
#define COLOR_DARKGREY  0x18E3
#define COLOR_LIGHTGREY 0x8410
#define COLOR_CYAN      0x07FF
#define COLOR_YELLOW    0xFFE0
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BORDER    0x3A59

/**
 * @brief Initialize the ILI9341 SPI display.
 */
void display_init(void);

/**
 * @brief Set the active address window for pixel writes.
 */
void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief Fill the entire screen with a single color.
 */
void display_clear(uint16_t color);

/**
 * @brief Fill a rectangle with a single color.
 */
void display_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Draw a single 16x16 Tetris block at grid coordinates (row 0..19, col 0..9).
 */
void display_draw_grid_block(uint8_t row, uint8_t col, uint16_t color);

/**
 * @brief Draw a single ASCII character (5x7 standard bitmap font).
 */
void display_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

/**
 * @brief Draw a string of text.
 */
void display_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);

/**
 * @brief Render the static HUD elements (borders, labels). Call once upon game start.
 */
void display_render_hud_static(void);

/**
 * @brief Differential renderer: Updates changed grid cells, falling piece, and HUD stats.
 *        Zero-flicker, sub-millisecond execution.
 */
void display_render_game(const tetris_game_t *game);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
