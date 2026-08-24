#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  BTN_LEFT = 0,
  BTN_RIGHT,
  BTN_ROTATE,
  BTN_COUNT // Must be last
} ButtonID_t;

// Initialize the GPIO pins for the buttons
void Buttons_Init(void);

// Read the current state of a button (true if physically pressed right now)
bool Button_IsDown(ButtonID_t btn);

// Check if a button was just pressed (transitioned from unpressed to pressed)
// This is typically what you want for Tetris movement
bool Button_WasPressed(ButtonID_t btn);

// Update button states (call this once per game loop/tick)
void Buttons_Update(void);

#endif // BUTTONS_H
