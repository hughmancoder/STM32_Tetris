/**
 * @file tetromino.c
 * @brief Pure C rotation tables and color mappings for the standard Tetrominoes.
 * 
 * PURPOSE & ARCHITECTURE:
 * -----------------------
 * Implements the static shape tables and lookup helpers for all 7 standard pieces.
 * Uses 4x4 bounding box bitmaps according to the official Tetris Super Rotation System (SRS)
 * conventions to ensure authentic gameplay feel.
 * 
 * Hardware-free: No embedded or platform-specific dependencies.
 */

#include "tetromino.h"

/**
 * @brief 4x4 matrix patterns for 7 pieces across 4 rotations: [Piece][Rotation][Row][Col].
 */
static const uint8_t TETROMINO_SHAPES[TETROMINO_COUNT][TETROMINO_ROTATIONS][TETROMINO_MATRIX_SIZE][TETROMINO_MATRIX_SIZE] = {
    // TETROMINO_NONE (All zeros)
    [TETROMINO_NONE] = {
        {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}
    },

    // TETROMINO_I (Cyan bar)
    [TETROMINO_I] = {
        // Rot 0
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 1 (90 deg)
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0}
        },
        // Rot 2 (180 deg)
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0}
        },
        // Rot 3 (270 deg)
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        }
    },

    // TETROMINO_J (Pink)
    [TETROMINO_J] = {
        // Rot 0
        {
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 1
        {
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 2
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        // Rot 3
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // TETROMINO_L (Orange)
    [TETROMINO_L] = {
        // Rot 0
        {
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 1
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rot 2
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 3
        {
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // TETROMINO_O (Yellow 2x2)
    [TETROMINO_O] = {
        // Rot 0..3 (Invariant under rotation)
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
    },

    // TETROMINO_S (Red)
    [TETROMINO_S] = {
        // Rot 0
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 1
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        // Rot 2
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 3
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // TETROMINO_T (Purple)
    [TETROMINO_T] = {
        // Rot 0
        {
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 1
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 2
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 3
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },

    // TETROMINO_Z (Green)
    [TETROMINO_Z] = {
        // Rot 0
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 1
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rot 2
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rot 3
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        }
    }
};

/**
 * @brief Color table indexed by tetromino_type_t.
 */
static const uint16_t TETROMINO_COLORS[TETROMINO_COUNT] = {
    [TETROMINO_NONE] = RGB565_BLACK,
    [TETROMINO_I]    = COLOR_TETROMINO_I,
    [TETROMINO_J]    = COLOR_TETROMINO_J,
    [TETROMINO_L]    = COLOR_TETROMINO_L,
    [TETROMINO_O]    = COLOR_TETROMINO_O,
    [TETROMINO_S]    = COLOR_TETROMINO_S,
    [TETROMINO_T]    = COLOR_TETROMINO_T,
    [TETROMINO_Z]    = COLOR_TETROMINO_Z,
};

uint8_t tetromino_get_cell(tetromino_type_t type, uint8_t rotation, uint8_t row, uint8_t col) {
    if (type >= TETROMINO_COUNT || row >= TETROMINO_MATRIX_SIZE || col >= TETROMINO_MATRIX_SIZE) {
        return 0;
    }
    return TETROMINO_SHAPES[type][rotation % TETROMINO_ROTATIONS][row][col];
}

uint16_t tetromino_get_color(tetromino_type_t type) {
    if (type >= TETROMINO_COUNT) {
        return RGB565_BLACK;
    }
    return TETROMINO_COLORS[type];
}
