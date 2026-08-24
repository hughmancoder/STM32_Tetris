#include "buttons.h"
#include "stm32f4xx_hal.h"

// Hardware Mapping
// Left: PA10 (D2)
// Right: PB3 (D3)
// Rotate: PB5 (D4)

typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin;
  bool is_down;
  bool was_pressed;
} ButtonState_t;

static ButtonState_t buttons[BTN_COUNT] = {
    [BTN_LEFT] = {GPIOA, GPIO_PIN_10, false, false},
    [BTN_RIGHT] = {GPIOB, GPIO_PIN_3, false, false},
    [BTN_ROTATE] = {GPIOB, GPIO_PIN_5, false, false},
};

void Buttons_Init(void) {
  // Enable Clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configure Left Button (PA10)
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure Right (PB3) and Rotate (PB5) Buttons
  GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Buttons_Update(void) {
  for (int i = 0; i < BTN_COUNT; i++) {
    // Read the pin. Because of pull-up, it is high when unpressed, low when pressed.
    GPIO_PinState state = HAL_GPIO_ReadPin(buttons[i].port, buttons[i].pin);
    bool currently_down = (state == GPIO_PIN_RESET);

    // Check for edge transition (wasn't down before, but is down now)
    buttons[i].was_pressed = (currently_down && !buttons[i].is_down);
    buttons[i].is_down = currently_down;
  }
}

bool Button_IsDown(ButtonID_t btn) {
  if (btn >= BTN_COUNT) return false;
  return buttons[btn].is_down;
}

bool Button_WasPressed(ButtonID_t btn) {
  if (btn >= BTN_COUNT) return false;
  return buttons[btn].was_pressed;
}
