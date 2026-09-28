/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "tetris_core.h"
#include "tetromino.h"
#include <stdbool.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* Button state */
/* Level 1 Bare-metal: Button state definitions */
#define BUTTON_RELEASED 0U
#define BUTTON_PRESSED 1U

/* USER CODE BEGIN PV */
// UART_HandleTypeDef huart2;
/* USER CODE END PV */

/* SysTick millisecond counter */
volatile uint32_t ms_ticks = 0;

// documented in STM arm M4 MCU programming manual
void systick_init(uint32_t sys_clk_hz) {
  // 1 mm tick
  SysTick->LOAD = (sys_clk_hz / 1000U) - 1U;
  SysTick->VAL = 0U;

  // Control aand Status register
  // Tells the timer to use the main processor clock rather than an external,
  // slower clock, enables the hardware exception request and turns the timer on
  // and starts the actual countdown.
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
                  SysTick_CTRL_ENABLE_Msk;
}

uint32_t get_millis(void) { return ms_ticks; }

void delay_ms(uint32_t ms) {
  uint32_t start = ms_ticks;
  while ((ms_ticks - start) < ms)
    ;
}

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// void MX_USART2_UART_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* --- Original HAL UART (Commented Out) ---
void MX_USART2_UART_Init(void) {
  __HAL_RCC_USART2_CLK_ENABLE();
  huart2.Instance = USART2;
  ...
}

int _write(int file, char *ptr, int len) {
  (void)file;
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}
--- End Original HAL UART --- */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Level 1 Bare-metal: Commented out HAL_Init() */
  // HAL_Init();

  /* Initialize SysTick timer for 1ms interrupts (default 16MHz HSI clock) */
  systick_init(16000000U);

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  // MX_USART2_UART_Init();

  display_init();

  tetris_game_t game;
  tetris_init(&game);
  display_render_game(&game);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_gravity_time = get_millis();
  uint32_t last_frame_time = get_millis();

  uint8_t prev_left = 1;
  uint8_t prev_right = 1;
  uint8_t prev_rotate = 1;
  uint8_t prev_drop = 1;

  while (true) {
    uint32_t now = get_millis();

    /* --- Original HAL ReadPin ---
    GPIO_PinState curr_left = HAL_GPIO_ReadPin(Btn_Left_GPIO_Port,
    Btn_Left_Pin); GPIO_PinState curr_right =
    HAL_GPIO_ReadPin(Btn_Right_GPIO_Port, Btn_Right_Pin); GPIO_PinState
    curr_rotate = HAL_GPIO_ReadPin(Btn_Rotate_GPIO_Port, Btn_Rotate_Pin);
    GPIO_PinState curr_drop = HAL_GPIO_ReadPin(Btn_Drop_GPIO_Port,
    Btn_Drop_Pin);
    ---------------------------------------------- */

    /* Read from GPIOC input data registers for button states */
    uint8_t curr_left = (GPIOC->IDR >> Btn_Left_Pin_Pos) & 1U;
    uint8_t curr_right = (GPIOC->IDR >> Btn_Right_Pin_Pos) & 1U;
    uint8_t curr_rotate = (GPIOC->IDR >> Btn_Rotate_Pin_Pos) & 1U;
    uint8_t curr_drop = (GPIOC->IDR >> Btn_Drop_Pin_Pos) & 1U;

    tetris_input_t input = TETRIS_INPUT_NONE;

    if (prev_left == 1 && curr_left == 0) {
      input |= TETRIS_INPUT_LEFT;
    }
    if (prev_right == 1 && curr_right == 0) {
      input |= TETRIS_INPUT_RIGHT;
    }
    if (prev_rotate == 1 && curr_rotate == 0) {
      input |= TETRIS_INPUT_ROTATE;
    }
    if (prev_drop == 1 && curr_drop == 0) {
      input |= TETRIS_INPUT_DROP;
    }

    prev_left = curr_left;
    prev_right = curr_right;
    prev_rotate = curr_rotate;
    prev_drop = curr_drop;

    // If game over and any button pressed, restart
    if (game.state == TETRIS_STATE_GAME_OVER && input != TETRIS_INPUT_NONE) {
      tetris_init(&game);
      display_render_game(&game);
      input = TETRIS_INPUT_NONE;
    }

    // Gravity Tick Calculation (speeds up with level)
    uint32_t gravity_interval = 800;
    if (game.level > 1 && game.level <= 10) {
      gravity_interval = 800 - (game.level - 1) * 70;
    } else if (game.level > 10) {
      gravity_interval = 120;
    }

    bool gravity_tick = false;
    if (now - last_gravity_time >= gravity_interval) {
      gravity_tick = true;
      last_gravity_time = now;
    }

    // Step Game Simulation
    if (input != TETRIS_INPUT_NONE || gravity_tick) {
      tetris_step(&game, input, gravity_tick);
    }

    //  every 25 ms (1/25e-3 = 40fps)
    if (now - last_frame_time >= 25) {
      last_frame_time = now;
      display_render_game(&game);
    }

    delay_ms(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  /* =========================================================================
   * Bare-Metal Clock Configuration (To be implemented by you if desired)
   * Refer to Reference Manual RM0390 -> Section: Reset and Clock Control (RCC)
   * By default on reset, the STM32F446 runs on the internal 16MHz HSI clock.
   * If configuring PLL (e.g. up to 180MHz):
   * 1. Enable PWR clock, configure voltage scaling (PWR->CR)
   * 2. Configure Flash latency wait states (FLASH->ACR)
   * 3. Configure PLL multipliers/dividers (RCC->PLLCFGR)
   * 4. Enable PLL and wait for PLLRDY (RCC->CR)
   * 5. Switch SYSCLK to PLL (RCC->CFGR)
   * =========================================================================
   */

  /* --- Original HAL Clock Configuration (Commented Out) ---
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  --- End Original HAL Clock Configuration --- */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* --- Original HAL EXTI Callback (Commented Out) ---
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == USER_BUTTON_PIN) {
    BspButtonState = BUTTON_PRESSED;
  }
}
--- End HAL EXTI Callback --- */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
