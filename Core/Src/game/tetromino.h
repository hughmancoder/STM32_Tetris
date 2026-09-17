/**
 * @file tetromino.h
 * @brief Pure C Tetromino definitions, rotation matrices, and color palettes.
 *
 * PURPOSE & ARCHITECTURE:
 * -----------------------
 * This file defines the core geometry and visual attributes of the 7 standard
 * Tetris pieces (I, J, L, O, S, T, Z) following the standard Tetris guideline.
 *
 * It is completely decoupled from any target hardware, STM32 HAL libraries,
 * display controllers, or operating systems. It only depends on standard C
 * types (<stdint.h>, <stdbool.h>).
 *
 * Each piece is defined across 4 rotation states (0 deg, 90 deg, 180 deg, 270
 * deg) within a normalized 4x4 matrix grid. Colors are defined in standard
 * 16-bit RGB565 format for direct mapping to color SPI TFT displays (e.g.
 * ILI9341, ST7789).
 */

#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TETROMINO_MATRIX_SIZE                                                  \
  4                           ///< Width and height of a tetromino bounding box
#define TETROMINO_ROTATIONS 4 ///< Number of 90-degree rotation states (0..3)

/**
 * @brief Standard 7 Tetromino types plus empty/none indicator.
 */
typedef enum {
  TETROMINO_NONE = 0,
  TETROMINO_I,    ///< Cyan 4-long bar
  TETROMINO_J,    ///< Pink J-hook
  TETROMINO_L,    ///< Orange L-hook
  TETROMINO_O,    ///< Yellow 2x2 square
  TETROMINO_S,    ///< Red S-snake
  TETROMINO_T,    ///< Purple T-shape
  TETROMINO_Z,    ///< Green Z-snake
  TETROMINO_COUNT ///< Total number of entries
} tetromino_type_t;

/**
 * @brief 16-bit RGB565 color definitions for authentic Tetris piece rendering.
 */
#define RGB565_BLACK 0x0000
#define RGB565_WHITE 0xFFFF
#define RGB565_GRAY 0x8410
#define RGB565_DARKGRAY 0x2104

#define COLOR_TETROMINO_I 0x0536 ///< Warm Teal / Turquoise (0, 168, 185)
#define COLOR_TETROMINO_J 0xDFA2 ///< Soft Coral Pink (230, 120, 150)
#define COLOR_TETROMINO_L 0xEC02 ///< Warm Tangerine Orange (245, 130, 20)
#define COLOR_TETROMINO_O 0xE602 ///< Warm Goldenrod Yellow (235, 195, 20)
#define COLOR_TETROMINO_S 0xD1A4 ///< Terra Cotta / Brick Red (215, 55, 40)
#define COLOR_TETROMINO_T 0x9262 ///< Muted Wood Plum / Purple (145, 75, 145)
#define COLOR_TETROMINO_Z 0x1468 ///< Forest / Emerald Green (20, 145, 70)

/**
 * @brief Get whether a cell at (row, col) inside the 4x4 bounding box is
 * occupied.
 *
 * @param[in] type Tetromino type (I, J, L, O, S, T, Z).
 * @param[in] rotation Rotation index (0 = 0 deg, 1 = 90 deg, 2 = 180 deg, 3 =
 * 270 deg).
 * @param[in] row Row inside the 4x4 bounding box (0..3).
 * @param[in] col Column inside the 4x4 bounding box (0..3).
 * @return uint8_t 1 if occupied, 0 if empty.
 */
uint8_t tetromino_get_cell(tetromino_type_t type, uint8_t rotation, uint8_t row,
                           uint8_t col);

/**
 * @brief Get the 16-bit RGB565 display color for a given tetromino type.
 *
 * @param[in] type Tetromino piece type.
 * @return uint16_t RGB565 color value.
 */
uint16_t tetromino_get_color(tetromino_type_t type);

#ifdef __cplusplus
}
#endif

#endif // TETROMINO_H
