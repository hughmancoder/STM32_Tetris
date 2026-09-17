/**
 * @file terminal_runner.c
 * @brief Terminal-based interactive runner for the pure-C Tetris game engine.
 *
 * PURPOSE & ARCHITECTURE:
 * -----------------------
 * This executable provides a visual game loop running directly in your
 * Mac/Linux terminal, completely independent of STM32 hardware.
 *
 * It validates that:
 * 1. The pure C engine in `src/game/` is 100% decoupled and host-executable.
 * 2. Lateral movement, SRS rotations, wall kicks, gravity, line clears, and
 * scoring work seamlessly.
 * 3. The exact same game logic that runs on hardware can be visualized, played,
 * and debugged locally.
 *
 * Controls:
 *   [A] or [Left Arrow]   : Move Left
 *   [D] or [Right Arrow]  : Move Right
 *   [W] or [Up Arrow]     : Rotate Clockwise
 *   [S] or [Down Arrow]   : Soft Drop
 *   [Space]               : Hard Drop
 *   [R]                   : Restart Game
 *   [Q]                   : Quit
 */

#include "tetris_core.h"
#include "tetromino.h"
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// Terminal state restoration
static struct termios s_orig_termios;
static bool s_raw_mode_enabled = false;

static void disable_raw_mode(void) {
  if (s_raw_mode_enabled) {
    // Show cursor and restore terminal mode
    printf("\033[?25h\033[0m\n");
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &s_orig_termios);
    s_raw_mode_enabled = false;
  }
}

static void sigint_handler(int sig) {
  (void)sig;
  disable_raw_mode();
  exit(0);
}

static void enable_raw_mode(void) {
  tcgetattr(STDIN_FILENO, &s_orig_termios);
  atexit(disable_raw_mode);
  signal(SIGINT, sigint_handler);

  struct termios raw = s_orig_termios;
  // Disable canonical mode (line buffering) and echo
  raw.c_lflag &= (unsigned)~(ECHO | ICANON);
  // Non-blocking read (return immediately if no input)
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSANOW, &raw);
  s_raw_mode_enabled = true;

  // Hide cursor and clear screen
  printf("\033[?25l\033[2J");
  fflush(stdout);
}

/**
 * @brief Get current timestamp in milliseconds using monotonic clock.
 */
static uint64_t get_time_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

/**
 * @brief Map tetromino type to ANSI terminal escape sequences (color + block).
 */
static const char *get_cell_ansi(uint8_t cell) {
  switch (cell) {
  case TETROMINO_I:
    return "\033[38;2;0;168;185m██\033[0m"; // Warm Teal / Turquoise (0, 168,
                                            // 185)
  case TETROMINO_J:
    return "\033[38;2;230;120;150m██\033[0m"; // Soft Coral Pink (230, 120, 150)
  case TETROMINO_L:
    return "\033[38;2;245;130;20m██\033[0m"; // Warm Tangerine Orange (245, 130,
                                             // 20)
  case TETROMINO_O:
    return "\033[38;2;235;195;20m██\033[0m"; // Warm Goldenrod Yellow (235, 195,
                                             // 20)
  case TETROMINO_S:
    return "\033[38;2;215;55;40m██\033[0m"; // Terra Cotta / Brick Red (215, 55,
                                            // 40)
  case TETROMINO_T:
    return "\033[38;2;145;75;145m██\033[0m"; // Muted Wood Plum / Purple (145,
                                             // 75, 145)
  case TETROMINO_Z:
    return "\033[38;2;20;145;70m██\033[0m"; // Forest / Emerald Green (20, 145,
                                            // 70)
  default:
    return "  "; // Empty cell
  }
}

/**
 * @brief Read non-blocking keyboard input and map to tetris_input_t.
 */
static tetris_input_t read_keyboard_input(bool *quit_requested) {
  tetris_input_t input = TETRIS_INPUT_NONE;
  char c;

  while (read(STDIN_FILENO, &c, 1) > 0) {
    if (c == 'q' || c == 'Q') {
      if (quit_requested)
        *quit_requested = true;
    } else if (c == 'a' || c == 'A') {
      input |= TETRIS_INPUT_LEFT;
    } else if (c == 'd' || c == 'D') {
      input |= TETRIS_INPUT_RIGHT;
    } else if (c == 'w' || c == 'W') {
      input |= TETRIS_INPUT_ROTATE;
    } else if (c == 's' || c == 'S') {
      input |= TETRIS_INPUT_DOWN;
    } else if (c == ' ') {
      input |= TETRIS_INPUT_DROP;
    } else if (c == 'r' || c == 'R') {
      input |= TETRIS_INPUT_START;
    } else if (c == '\033') { // Escape sequence (e.g. arrow keys)
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) > 0 &&
          read(STDIN_FILENO, &seq[1], 1) > 0) {
        if (seq[0] == '[') {
          switch (seq[1]) {
          case 'A':
            input |= TETRIS_INPUT_ROTATE;
            break; // Up Arrow
          case 'B':
            input |= TETRIS_INPUT_DOWN;
            break; // Down Arrow
          case 'C':
            input |= TETRIS_INPUT_RIGHT;
            break; // Right Arrow
          case 'D':
            input |= TETRIS_INPUT_LEFT;
            break; // Left Arrow
          }
        }
      }
    }
  }

  return input;
}

/**
 * @brief Render the game state to the terminal using ANSI escape codes.
 */
static void render_terminal(const tetris_game_t *game) {
  uint8_t fb[TETRIS_BOARD_ROWS][TETRIS_BOARD_COLS];
  tetris_get_framebuffer(game, fb);

  // Reset cursor to top-left of terminal (flicker-free redraw)
  printf("\033[H");

  // Top border of playfield
  printf("  \033[1;34m╔════════════════════╗\033[0m   \033[1;37mHUD "
         "STATS\033[0m\n");

  for (int r = 0; r < TETRIS_BOARD_ROWS; r++) {
    // Left border
    printf("  \033[1;34m║\033[0m");

    // Row cells
    for (int c = 0; c < TETRIS_BOARD_COLS; c++) {
      printf("%s", get_cell_ansi(fb[r][c]));
    }

    // Right border
    printf("\033[1;34m║\033[0m   ");

    // Sidebar info lines
    switch (r) {
    case 1:
      printf("\033[1;33mSCORE:\033[0m %06u", game->score);
      break;
    case 2:
      printf("\033[1;33mHIGH:\033[0m  %06u", game->high_score);
      break;
    case 4:
      printf("\033[1;36mLEVEL:\033[0m %u", game->level);
      break;
    case 5:
      printf("\033[1;36mLINES:\033[0m %u", game->lines_cleared);
      break;
    case 7:
      printf("\033[1;35mNEXT PIECE:\033[0m");
      break;
    case 8:
    case 9:
    case 10:
    case 11: {
      int preview_row = r - 8;
      printf("  ");
      for (int pc = 0; pc < TETROMINO_MATRIX_SIZE; pc++) {
        if (tetromino_get_cell(game->next_piece, 0, preview_row, pc)) {
          printf("%s", get_cell_ansi((uint8_t)game->next_piece));
        } else {
          printf("  ");
        }
      }
      break;
    }
    case 14:
      printf("\033[90mCONTROLS:\033[0m");
      break;
    case 15:
      printf("\033[90mA / D / ← → : Move\033[0m");
      break;
    case 16:
      printf("\033[90mW / ↑       : Rotate\033[0m");
      break;
    case 17:
      printf("\033[90mS / ↓       : Soft Drop\033[0m");
      break;
    case 18:
      printf("\033[90mSpace       : Hard Drop\033[0m");
      break;
    case 19:
      printf("\033[90mR: Reset | Q: Quit\033[0m");
      break;
    default:
      break;
    }

    printf("\033[K\n"); // Clear to end of line
  }

  // Bottom border of playfield
  printf("  \033[1;34m╚════════════════════╝\033[0m\n");

  if (game->state == TETRIS_STATE_GAME_OVER) {
    printf("\n  \033[1;41;37m   *** GAME OVER! ***   \033[0m Press "
           "\033[1;33m[R]\033[0m to restart\n");
  }

  fflush(stdout);
}

int main(void) {
  tetris_game_t game;
  tetris_init(&game);

  enable_raw_mode();

  uint64_t last_gravity_time = get_time_ms();
  uint64_t last_frame_time = get_time_ms();
  bool quit = false;

  while (!quit) {
    uint64_t now = get_time_ms();

    // 1. Process Non-blocking User Input
    tetris_input_t input = read_keyboard_input(&quit);
    if (quit)
      break;

    // 2. Compute Gravity Interval (increases speed with level)
    uint64_t gravity_interval_ms = 800;
    if (game.level > 1 && game.level <= 10) {
      gravity_interval_ms = (uint64_t)(800 - (game.level - 1) * 70);
    } else if (game.level > 10) {
      gravity_interval_ms = 120;
    }

    bool gravity_tick = false;
    if (now - last_gravity_time >= gravity_interval_ms) {
      gravity_tick = true;
      last_gravity_time = now;
    }

    // 3. Advance Pure C Game Simulation Tick
    tetris_step(&game, input, gravity_tick);

    // 4. Render to Terminal at ~30-60 FPS
    if (now - last_frame_time >= 33) {
      last_frame_time = now;
      render_terminal(&game);
    }

    // Yield CPU briefly (10ms)
    usleep(10000);
  }

  disable_raw_mode();
  printf("\nExited Tetris Simulator. Thanks for playing!\n");
  return 0;
}
