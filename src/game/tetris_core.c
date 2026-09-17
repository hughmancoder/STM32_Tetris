/**
 * @file tetris_core.c
 * @brief Pure C Implementation of Tetris Game Rules, Movement, and Line Clears.
 *
 * PURPOSE & ARCHITECTURE:
 * -----------------------
 * Implements the core simulation logic for standard 10x20 Tetris:
 * - Deterministic state machine (`TETRIS_STATE_TITLE`, `PLAYING`, `GAME_OVER`).
 * - Wall and floor collision detection with obstacle verification.
 * - Tetromino lateral shift, 90-degree clockwise rotation, soft drop, and hard
 * drop.
 * - Lock-down into static board grid upon landing.
 * - Line clearing with row collapsing and standard Tetris scoring (Single,
 * Double, Triple, Tetris).
 * - 7-Bag pseudo-random piece generator for fair piece distribution.
 * - Framebuffer composition for hardware-agnostic rendering.
 *
 * Zero embedded dependencies: uses standard C99 only.
 */

#include "tetris_core.h"
#include <string.h>

/* Score points for clearing 0, 1, 2, 3, or 4 lines simultaneously */
static const uint16_t SCORE_TABLE[5] = {0, 100, 300, 500, 800};

/* Internal simple 7-bag randomizer state */
static tetromino_type_t s_bag[7];
static uint8_t s_bag_index = 7;
static uint32_t s_rng_state = 123456789;

/**
 * @brief Lightweight portable pseudo-random number generator (Xorshift32).
 */
static uint32_t xorshift32(void) {
  s_rng_state ^= s_rng_state << 13;
  s_rng_state ^= s_rng_state >> 17;
  s_rng_state ^= s_rng_state << 5;
  return s_rng_state;
}

/**
 * @brief Refill and shuffle the 7-bag of pieces.
 */
static void refill_bag(void) {
  for (int i = 0; i < 7; i++) {
    s_bag[i] = (tetromino_type_t)(TETROMINO_I + i);
  }
  // Fisher-Yates shuffle
  for (int i = 6; i > 0; i--) {
    uint32_t j = xorshift32() % (i + 1);
    tetromino_type_t temp = s_bag[i];
    s_bag[i] = s_bag[j];
    s_bag[j] = temp;
  }
  s_bag_index = 0;
}

/**
 * @brief Pull the next piece from the 7-bag.
 */
static tetromino_type_t get_next_bag_piece(void) {
  if (s_bag_index >= 7) {
    refill_bag();
  }
  return s_bag[s_bag_index++];
}

void tetris_init(tetris_game_t *game) {
  if (!game)
    return;

  memset(game->board, 0, sizeof(game->board));
  game->score = 0;
  game->high_score = 0;
  game->lines_cleared = 0;
  game->level = 1;
  game->state = TETRIS_STATE_PLAYING;
  game->board_changed = true;

  refill_bag();
  game->next_piece = get_next_bag_piece();
  tetris_spawn_piece(game);
}

bool tetris_check_collision(const tetris_game_t *game, tetromino_type_t type,
                            uint8_t rot, int8_t x, int8_t y) {
  if (!game || type == TETROMINO_NONE)
    return true;

  for (uint8_t r = 0; r < TETROMINO_MATRIX_SIZE; r++) {
    for (uint8_t c = 0; c < TETROMINO_MATRIX_SIZE; c++) {
      if (tetromino_get_cell(type, rot, r, c)) {
        int8_t board_x = x + c;
        int8_t board_y = y + r;

        // Check board boundaries (Left, Right, Floor)
        if (board_x < 0 || board_x >= TETRIS_BOARD_COLS)
          return true;
        if (board_y >= TETRIS_BOARD_ROWS)
          return true;

        // Check collision with already locked blocks
        if (board_y >= 0 && game->board[board_y][board_x] != TETROMINO_NONE) {
          return true;
        }
      }
    }
  }
  return false;
}

bool tetris_spawn_piece(tetris_game_t *game) {
  if (!game)
    return false;

  game->active_piece = game->next_piece;
  game->next_piece = get_next_bag_piece();
  game->active_rotation = 0;

  // Spawn top center: col 3 puts 4x4 matrix centered across 10 columns
  game->active_x = 3;
  game->active_y = 0;

  // Check if initial spawn position collides (Top-Out / Game Over)
  if (tetris_check_collision(game, game->active_piece, game->active_rotation,
                             game->active_x, game->active_y)) {
    game->state = TETRIS_STATE_GAME_OVER;
    return false;
  }
  return true;
}

/**
 * @brief Lock active piece into the static board grid.
 */
static void lock_piece(tetris_game_t *game) {
  for (uint8_t r = 0; r < TETROMINO_MATRIX_SIZE; r++) {
    for (uint8_t c = 0; c < TETROMINO_MATRIX_SIZE; c++) {
      if (tetromino_get_cell(game->active_piece, game->active_rotation, r, c)) {
        int8_t board_x = game->active_x + c;
        int8_t board_y = game->active_y + r;
        if (board_x >= 0 && board_x < TETRIS_BOARD_COLS && board_y >= 0 &&
            board_y < TETRIS_BOARD_ROWS) {
          game->board[board_y][board_x] = (uint8_t)game->active_piece;
        }
      }
    }
  }
}

/**
 * @brief Scan and clear any fully occupied rows, shifting upper rows down.
 * @return Number of lines cleared (0..4).
 */
static uint8_t clear_full_lines(tetris_game_t *game) {
  uint8_t lines_cleared = 0;

  for (int8_t r = TETRIS_BOARD_ROWS - 1; r >= 0; r--) {
    bool full_line = true;
    for (uint8_t c = 0; c < TETRIS_BOARD_COLS; c++) {
      if (game->board[r][c] == TETROMINO_NONE) {
        full_line = false;
        break;
      }
    }

    if (full_line) {
      lines_cleared++;
      // Shift all rows above down by one
      for (int8_t shift_r = r; shift_r > 0; shift_r--) {
        for (uint8_t c = 0; c < TETRIS_BOARD_COLS; c++) {
          game->board[shift_r][c] = game->board[shift_r - 1][c];
        }
      }
      // Clear the very top row
      for (uint8_t c = 0; c < TETRIS_BOARD_COLS; c++) {
        game->board[0][c] = TETROMINO_NONE;
      }
      // Re-check current row index as upper rows shifted down
      r++;
    }
  }

  if (lines_cleared > 0 && lines_cleared <= 4) {
    game->score += (uint32_t)SCORE_TABLE[lines_cleared] * game->level;
    if (game->score > game->high_score) {
      game->high_score = game->score;
    }
    game->lines_cleared += lines_cleared;
    game->level = (uint8_t)(1 + (game->lines_cleared / 10));
  }

  return lines_cleared;
}

void tetris_hard_drop(tetris_game_t *game) {
  if (!game || game->state != TETRIS_STATE_PLAYING)
    return;

  // Drop downward until collision
  while (!tetris_check_collision(game, game->active_piece,
                                 game->active_rotation, game->active_x,
                                 game->active_y + 1)) {
    game->active_y++;
    game->score += 2; // Standard bonus for hard drop
  }

  lock_piece(game);
  clear_full_lines(game);
  tetris_spawn_piece(game);
  game->board_changed = true;
}

bool tetris_step(tetris_game_t *game, tetris_input_t inputs,
                 bool gravity_tick) {
  if (!game)
    return false;

  // Handle restart from Game Over
  if (game->state == TETRIS_STATE_GAME_OVER) {
    if (inputs &
        (TETRIS_INPUT_START | TETRIS_INPUT_ROTATE | TETRIS_INPUT_DROP)) {
      tetris_init(game);
    }
    return false;
  }

  if (game->state != TETRIS_STATE_PLAYING)
    return false;

  bool moved = false;

  // 1. Lateral Movement (Left / Right)
  if (inputs & TETRIS_INPUT_LEFT) {
    if (!tetris_check_collision(game, game->active_piece, game->active_rotation,
                                game->active_x - 1, game->active_y)) {
      game->active_x--;
      moved = true;
    }
  }
  if (inputs & TETRIS_INPUT_RIGHT) {
    if (!tetris_check_collision(game, game->active_piece, game->active_rotation,
                                game->active_x + 1, game->active_y)) {
      game->active_x++;
      moved = true;
    }
  }

  // 2. Rotation (Clockwise with simple wall kick)
  if (inputs & TETRIS_INPUT_ROTATE) {
    uint8_t next_rot = (game->active_rotation + 1) % TETROMINO_ROTATIONS;
    if (!tetris_check_collision(game, game->active_piece, next_rot,
                                game->active_x, game->active_y)) {
      game->active_rotation = next_rot;
      moved = true;
    } else if (!tetris_check_collision(game, game->active_piece, next_rot,
                                       game->active_x - 1, game->active_y)) {
      // Simple left wall kick
      game->active_x--;
      game->active_rotation = next_rot;
      moved = true;
    } else if (!tetris_check_collision(game, game->active_piece, next_rot,
                                       game->active_x + 1, game->active_y)) {
      // Simple right wall kick
      game->active_x++;
      game->active_rotation = next_rot;
      moved = true;
    }
  }

  // 3. Hard Drop
  if (inputs & TETRIS_INPUT_DROP) {
    tetris_hard_drop(game);
    return true;
  }

  // 4. Soft Drop / Gravity Tick
  bool step_down = gravity_tick || (inputs & TETRIS_INPUT_DOWN);
  if (step_down) {
    if (!tetris_check_collision(game, game->active_piece, game->active_rotation,
                                game->active_x, game->active_y + 1)) {
      game->active_y++;
      if (inputs & TETRIS_INPUT_DOWN) {
        game->score += 1; // Soft drop bonus
      }
      moved = true;
    } else {
      // Landed: lock piece, clear lines, and spawn next
      lock_piece(game);
      clear_full_lines(game);
      tetris_spawn_piece(game);
      moved = true;
    }
  }

  if (moved) {
    game->board_changed = true;
  }

  return (game->state == TETRIS_STATE_PLAYING);
}

void tetris_get_framebuffer(
    const tetris_game_t *game,
    uint8_t out_grid[TETRIS_BOARD_ROWS][TETRIS_BOARD_COLS]) {
  if (!game || !out_grid)
    return;

  // 1. Copy locked board state
  memcpy(out_grid, game->board, sizeof(game->board));

  // 2. Overlay active piece if actively playing
  if (game->state == TETRIS_STATE_PLAYING &&
      game->active_piece != TETROMINO_NONE) {
    for (uint8_t r = 0; r < TETROMINO_MATRIX_SIZE; r++) {
      for (uint8_t c = 0; c < TETROMINO_MATRIX_SIZE; c++) {
        if (tetromino_get_cell(game->active_piece, game->active_rotation, r,
                               c)) {
          int8_t bx = game->active_x + c;
          int8_t by = game->active_y + r;
          if (bx >= 0 && bx < TETRIS_BOARD_COLS && by >= 0 &&
              by < TETRIS_BOARD_ROWS) {
            out_grid[by][bx] = (uint8_t)game->active_piece;
          }
        }
      }
    }
  }
}
