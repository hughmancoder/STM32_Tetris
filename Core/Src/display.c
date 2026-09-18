/**
 * @file display.c
 * @brief High-performance SPI TFT display driver (ILI9341) and Tetris renderer.
 */

#include "display.h"
#include <stdio.h>
#include <string.h>

#define CS_LOW() HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)
#define DC_CMD() HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET)
#define DC_DATA() HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET)
#define RST_LOW() HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET)
#define RST_HIGH() HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET)

// Low-level SPI transfer helpers
static inline void write_cmd(uint8_t cmd) {
  DC_CMD();
  CS_LOW();
  HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
  CS_HIGH();
}

static inline void write_data(uint8_t data) {
  DC_DATA();
  CS_LOW();
  HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
  CS_HIGH();
}

static inline void write_data_buf(const uint8_t *buf, uint16_t len) {
  DC_DATA();
  CS_LOW();
  HAL_SPI_Transmit(&hspi1, (uint8_t *)buf, len, HAL_MAX_DELAY);
  CS_HIGH();
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  write_cmd(0x2A); // CASET
  uint8_t data_x[] = {(uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF),
                      (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF)};
  write_data_buf(data_x, 4);

  write_cmd(0x2B); // PASET
  uint8_t data_y[] = {(uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
                      (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF)};
  write_data_buf(data_y, 4);

  write_cmd(0x2C); // RAMWR
}

void display_init(void) {
  // 1. Hardware Reset
  RST_HIGH();
  HAL_Delay(5);
  RST_LOW();
  HAL_Delay(20);
  RST_HIGH();
  HAL_Delay(150);

  // 2. Software Reset
  write_cmd(0x01);
  HAL_Delay(120);

  // 3. ILI9341 Initialization Sequence
  write_cmd(0xCB);
  {
    uint8_t d[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
    write_data_buf(d, 5);
  }

  write_cmd(0xCF);
  {
    uint8_t d[] = {0x00, 0xC1, 0x30};
    write_data_buf(d, 3);
  }

  write_cmd(0xE8);
  {
    uint8_t d[] = {0x85, 0x00, 0x78};
    write_data_buf(d, 3);
  }

  write_cmd(0xEA);
  {
    uint8_t d[] = {0x00, 0x00};
    write_data_buf(d, 2);
  }

  write_cmd(0xED);
  {
    uint8_t d[] = {0x64, 0x03, 0x12, 0x81};
    write_data_buf(d, 4);
  }

  write_cmd(0xF7);
  write_data(0x20);

  write_cmd(0xC0); // Power Control 1
  write_data(0x23);

  write_cmd(0xC1); // Power Control 2
  write_data(0x10);

  write_cmd(0xC5); // VCOM Control 1
  write_data(0x3E);
  write_data(0x28);

  write_cmd(0xC7); // VCOM Control 2
  write_data(0x86);

  write_cmd(0x36); // Memory Access Control (Portrait Mode, BGR subpixel layout)
  write_data(0x48); // MX=1, BGR=1 (Required for this physical TFT panel to map
                    // RGB565 correctly)

  write_cmd(0x3A); // Pixel Format (16 bits/pixel)
  write_data(0x55);

  write_cmd(0xB1); // Frame Rate Control
  write_data(0x00);
  write_data(0x18);

  write_cmd(0xB6); // Display Function Control
  {
    uint8_t d[] = {0x08, 0x82, 0x27};
    write_data_buf(d, 3);
  }

  write_cmd(0x11); // Sleep OUT
  HAL_Delay(120);

  write_cmd(0x29); // Display ON
  HAL_Delay(20);

  display_clear(COLOR_BLACK);
}

void display_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                       uint16_t color) {
  if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT || w == 0 || h == 0)
    return;
  if (x + w > DISPLAY_WIDTH)
    w = DISPLAY_WIDTH - x;
  if (y + h > DISPLAY_HEIGHT)
    h = DISPLAY_HEIGHT - y;

  display_set_window(x, y, x + w - 1, y + h - 1);

  uint8_t hi = (uint8_t)(color >> 8);
  uint8_t lo = (uint8_t)(color & 0xFF);

#define CHUNK_PIXELS 32
  uint8_t chunk[CHUNK_PIXELS * 2];
  for (int i = 0; i < CHUNK_PIXELS; i++) {
    chunk[i * 2] = hi;
    chunk[i * 2 + 1] = lo;
  }

  uint32_t total_pixels = (uint32_t)w * h;
  DC_DATA();
  CS_LOW();
  while (total_pixels > 0) {
    uint16_t count =
        (total_pixels > CHUNK_PIXELS) ? CHUNK_PIXELS : (uint16_t)total_pixels;
    HAL_SPI_Transmit(&hspi1, chunk, count * 2, HAL_MAX_DELAY);
    total_pixels -= count;
  }
  CS_HIGH();
}

void display_clear(uint16_t color) {
  display_fill_rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, color);
}

void display_draw_grid_block(uint8_t row, uint8_t col, uint16_t color) {
  if (row >= TETRIS_BOARD_ROWS || col >= TETRIS_BOARD_COLS)
    return;

  uint16_t bx = PLAYFIELD_X + col * BLOCK_SIZE;
  uint16_t by = PLAYFIELD_Y + row * BLOCK_SIZE;

  if (color == COLOR_BLACK) {
    display_fill_rect(bx, by, BLOCK_SIZE, BLOCK_SIZE, COLOR_BLACK);
  } else {
    // Draw solid vibrant block in the true tetromino color
    display_fill_rect(bx, by, BLOCK_SIZE - 1, BLOCK_SIZE - 1, color);
    // 1px black seam between adjacent blocks for clean puzzle-piece definition
    display_fill_rect(bx + BLOCK_SIZE - 1, by, 1, BLOCK_SIZE, COLOR_BLACK);
    display_fill_rect(bx, by + BLOCK_SIZE - 1, BLOCK_SIZE, 1, COLOR_BLACK);
  }
}

// 5x7 Basic Bitmap Font Table (ASCII 32 ' ' to 90 'Z')
static const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')'
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0' (48)
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';'
    {0x00, 0x08, 0x14, 0x22, 0x41}, // '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '='
    {0x41, 0x22, 0x14, 0x08, 0x00}, // '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A' (65)
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // 'F'
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 'Z'
};

void display_draw_char(uint16_t x, uint16_t y, char c, uint16_t color,
                       uint16_t bg, uint8_t size) {
  if (c >= 'a' && c <= 'z')
    c -= 32; // Convert lowercase to uppercase
  if (c < 32 || c > 90)
    c = ' ';

  const uint8_t *glyph = font5x7[c - 32];
  for (uint8_t i = 0; i < 5; i++) {
    uint8_t line = glyph[i];
    for (uint8_t j = 0; j < 8; j++) {
      uint16_t pixel_color = (line & 0x01) ? color : bg;
      if (size == 1) {
        display_fill_rect(x + i, y + j, 1, 1, pixel_color);
      } else {
        display_fill_rect(x + i * size, y + j * size, size, size, pixel_color);
      }
      line >>= 1;
    }
  }
  // 1px trailing space between characters
  if (size == 1) {
    display_fill_rect(x + 5, y, 1, 8, bg);
  } else {
    display_fill_rect(x + 5 * size, y, size, 8 * size, bg);
  }
}

void display_draw_string(uint16_t x, uint16_t y, const char *str,
                         uint16_t color, uint16_t bg, uint8_t size) {
  uint16_t cursor_x = x;
  while (*str) {
    display_draw_char(cursor_x, y, *str++, color, bg, size);
    cursor_x += 6 * size;
  }
}

void display_render_hud_static(void) {
  // Vertical playfield divider line
  display_fill_rect(PLAYFIELD_W, 0, 2, DISPLAY_HEIGHT, COLOR_BORDER);

  // Background of HUD
  display_fill_rect(HUD_X + 2, 0, HUD_W - 2, DISPLAY_HEIGHT, COLOR_NAVY);

  // Static Headers
  display_draw_string(HUD_X + 6, 12, "TETRIS", COLOR_YELLOW, COLOR_NAVY, 2);
  display_draw_string(HUD_X + 6, 40, "SCORE", COLOR_LIGHTGREY, COLOR_NAVY, 1);
  display_draw_string(HUD_X + 6, 85, "LEVEL", COLOR_LIGHTGREY, COLOR_NAVY, 1);
  display_draw_string(HUD_X + 6, 130, "LINES", COLOR_LIGHTGREY, COLOR_NAVY, 1);
  display_draw_string(HUD_X + 6, 175, "NEXT", COLOR_LIGHTGREY, COLOR_NAVY, 1);
}

// Differential rendering cache
static uint8_t s_prev_grid[TETRIS_BOARD_ROWS][TETRIS_BOARD_COLS];
static uint32_t s_prev_score = 0xFFFFFFFF;
static uint16_t s_prev_lines = 0xFFFF;
static uint8_t s_prev_level = 0xFF;
static tetromino_type_t s_prev_next = TETROMINO_NONE;
static bool s_first_frame = true;

void display_render_game(const tetris_game_t *game) {
  if (s_first_frame) {
    display_clear(COLOR_BLACK);
    display_render_hud_static();
    memset(s_prev_grid, 0xFF, sizeof(s_prev_grid));
    s_first_frame = false;
  }

  // 1. Differential Grid Update
  uint8_t curr_grid[TETRIS_BOARD_ROWS][TETRIS_BOARD_COLS];
  tetris_get_framebuffer(game, curr_grid);

  for (uint8_t r = 0; r < TETRIS_BOARD_ROWS; r++) {
    for (uint8_t c = 0; c < TETRIS_BOARD_COLS; c++) {
      if (curr_grid[r][c] != s_prev_grid[r][c]) {
        uint16_t col_rgb =
            (curr_grid[r][c] == 0)
                ? COLOR_BLACK
                : tetromino_get_color((tetromino_type_t)curr_grid[r][c]);
        display_draw_grid_block(r, c, col_rgb);
        s_prev_grid[r][c] = curr_grid[r][c];
      }
    }
  }

  // 2. Score Update
  if (game->score != s_prev_score) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%06lu", (unsigned long)game->score);
    display_draw_string(HUD_X + 6, 55, buf, COLOR_WHITE, COLOR_NAVY, 1);
    s_prev_score = game->score;
  }

  // 3. Level Update
  if (game->level != s_prev_level) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02u", game->level);
    display_draw_string(HUD_X + 6, 100, buf, COLOR_WHITE, COLOR_NAVY, 2);
    s_prev_level = game->level;
  }

  // 4. Lines Update
  if (game->lines_cleared != s_prev_lines) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%03u", game->lines_cleared);
    display_draw_string(HUD_X + 6, 145, buf, COLOR_WHITE, COLOR_NAVY, 1);
    s_prev_lines = game->lines_cleared;
  }

  // 5. Next Piece Preview Update
  if (game->next_piece != s_prev_next) {
    uint16_t px = HUD_X + 10;
    uint16_t py = 195;
    // Clear 4x4 preview box (each block 10x10 px)
    display_fill_rect(px, py, 40, 40, COLOR_NAVY);

    uint16_t next_col = tetromino_get_color(game->next_piece);
    for (uint8_t r = 0; r < TETROMINO_MATRIX_SIZE; r++) {
      for (uint8_t c = 0; c < TETROMINO_MATRIX_SIZE; c++) {
        if (tetromino_get_cell(game->next_piece, 0, r, c)) {
          display_fill_rect(px + c * 10, py + r * 10, 9, 9, next_col);
        }
      }
    }
    s_prev_next = game->next_piece;
  }

  // 6. Game Over Banner
  if (game->state == TETRIS_STATE_GAME_OVER) {
    display_fill_rect(10, 140, 140, 35, COLOR_RED);
    display_draw_string(25, 150, "GAME OVER", COLOR_WHITE, COLOR_RED, 2);
  }
}
