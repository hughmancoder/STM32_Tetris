/**
 * @file tetris_core.h
 * @brief Decoupled, Pure-C Core Tetris Game Engine.
 *
 * PURPOSE & ARCHITECTURE:
 * -----------------------
 * This header defines the public contract, data models, and state machine
 * for the Tetris game engine.
 *
 * DESIGN PRINCIPLES:
 * 1. Hardware Agnostic: ZERO dependencies on STM32 HAL, SPI, GPIO, or displays.
 *    Only standard C types (<stdint.h>, <stdbool.h>) are used.
 * 2. Fully Testable: Can be compiled and executed on a host PC/Mac under
 * GCC/Clang for instant unit testing without hardware.
 * 3. Standard Playfield: 10 columns x 20 visible rows.
 * 4. Deterministic Tick: State transitions occur strictly through
 * `tetris_step()`, taking an abstract input bitmask and gravity tick indicator.
 * 5. Clean Rendering Boundary: Exposes `tetris_get_framebuffer()` or direct
 * state inspection so the display adapter can draw cells without the game
 * knowing how pixels are pushed.
 */

#ifndef TETRIS_CORE_H
#define TETRIS_CORE_H

#include "tetromino.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TETRIS_BOARD_ROWS 20 ///< Standard visible height
#define TETRIS_BOARD_COLS 10 ///< Standard playfield width

typedef enum {
  TETRIS_INPUT_NONE = 0,
  TETRIS_INPUT_LEFT = (1 << 0),   ///< Shift active piece left by 1 column
  TETRIS_INPUT_RIGHT = (1 << 1),  ///< Shift active piece right by 1 column
  TETRIS_INPUT_ROTATE = (1 << 2), ///< Rotate active piece clockwise (90 deg)
  TETRIS_INPUT_DOWN = (1 << 3),   ///< Soft drop (fast fall)
  TETRIS_INPUT_DROP = (1 << 4),   ///< Hard drop (instant drop to bottom & lock)
  TETRIS_INPUT_START = (1 << 5)   ///< Start / Restart trigger
} tetris_input_t;

/**
 * @brief High-level game state machine states.
 */
typedef enum {
  TETRIS_STATE_TITLE = 0, ///< Waiting to start (title/attract screen)
  TETRIS_STATE_PLAYING,   ///< Active gameplay
  TETRIS_STATE_GAME_OVER  ///< Board topped out; waiting for restart
} tetris_state_t;

typedef struct {
  uint8_t board[TETRIS_BOARD_ROWS][TETRIS_BOARD_COLS];
  tetromino_type_t active_piece; ///< Currently falling tetromino
  uint8_t active_rotation;       ///< Rotation index (0..3)
  int8_t active_x; ///< Column position of 4x4 bounding box (can be negative
                   ///< during wall tests)
  int8_t active_y; ///< Row position of 4x4 bounding box

  tetromino_type_t next_piece; ///< Next upcoming piece for HUD preview

  uint32_t score;         ///< Current cumulative score
  uint32_t high_score;    ///< Session high score
  uint16_t lines_cleared; ///< Total lines cleared
  uint8_t level;          ///< Current difficulty level

  tetris_state_t state; ///< Current engine state
  bool board_changed;   ///< Dirty flag set when board or HUD needs redraw
} tetris_game_t;

/**
 * @brief Initialize or reset the game engine to default state.
 *
 * @param[out] game Pointer to game instance to initialize.
 */
void tetris_init(tetris_game_t *game);

/**
 * @brief Advance the game engine by one tick.
 *
 * @param[in,out] game Pointer to game instance.
 * @param[in] inputs Bitmask of user input events triggered during this tick.
 * @param[in] gravity_tick True if a gravity drop interval has elapsed.
 * @return true if game is actively running, false if game over or title screen.
 */
bool tetris_step(tetris_game_t *game, tetris_input_t inputs, bool gravity_tick);

/**
 * @brief Check if a piece at a proposed position/rotation would collide with
 * walls or locked cells.
 *
 * @param[in] game Pointer to game instance.
 * @param[in] type Tetromino type to test.
 * @param[in] rot Rotation index (0..3).
 * @param[in] x Proposed grid column position.
 * @param[in] y Proposed grid row position.
 * @return true if collision detected (invalid position), false if free.
 */
bool tetris_check_collision(const tetris_game_t *game, tetromino_type_t type,
                            uint8_t rot, int8_t x, int8_t y);

/**
 * @brief Spawns the next piece at the top-center of the board.
 *
 * @param[in,out] game Pointer to game instance.
 * @return true if spawned cleanly, false if blocked (causes Game Over).
 */
bool tetris_spawn_piece(tetris_game_t *game);

/**
 * @brief Instantly drop the active piece to the lowest valid row and lock it.
 *
 * @param[in,out] game Pointer to game instance.
 */
void tetris_hard_drop(tetris_game_t *game);

/**
 * @brief Compose the combined playfield frame (locked grid + active falling
 * piece). Used by the display driver to draw the current visual state.
 *
 * @param[in] game Pointer to game instance.
 * @param[out] out_grid 2D array [ROWS][COLS] receiving the cell types (0 =
 * empty, 1..7 = piece color type).
 */
void tetris_get_framebuffer(
    const tetris_game_t *game,
    uint8_t out_grid[TETRIS_BOARD_ROWS][TETRIS_BOARD_COLS]);

#ifdef __cplusplus
}
#endif

#endif // TETRIS_CORE_H
