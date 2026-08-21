#include "sos.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

static void blink_led(int duration_ms) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
  printf("LED is ON  (PA5 State: %d)\r\n", HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5));
  HAL_Delay(duration_ms);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  printf("LED is OFF (PA5 State: %d)\r\n", HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5));
  HAL_Delay(250); // Pause between blinks
}

void play_sos(void) {
  int i;

  // S: 3 short blinks (200ms)
  for (i = 0; i < 3; i++) {
    blink_led(200);
  }

  HAL_Delay(300); // Extra pause between letters
  for (i = 0; i < 3; i++) {
    blink_led(600);
  }

  HAL_Delay(300); // Extra pause between letters

  // S: 3 short blinks (200ms)
  for (i = 0; i < 3; i++) {
    blink_led(200);
  }

  // Long pause before repeating the whole SOS message
  HAL_Delay(2000);
}
