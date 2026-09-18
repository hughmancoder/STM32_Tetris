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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "display.h"
#include "tetris_core.h"
#include "tetromino.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* Button state */
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

__IO uint32_t BspButtonState = BUTTON_RELEASED;

/* USER CODE BEGIN PV */
UART_HandleTypeDef huart2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MX_USART2_UART_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MX_USART2_UART_Init(void)
{
  __HAL_RCC_USART2_CLK_ENABLE();
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

int _write(int file, char *ptr, int len)
{
  (void)file;
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  MX_USART2_UART_Init();
  printf("\r\n========================================\r\n");
  printf("     STM32 Tetris Game Starting!        \r\n");
  printf("  Controls:                             \r\n");
  printf("    Left   : PC0 (A5)                   \r\n");
  printf("    Right  : PC1 (A4)                   \r\n");
  printf("    Rotate : PC2 / Blue Onboard Button  \r\n");
  printf("    Drop   : PC3                        \r\n");
  printf("========================================\r\n");

  display_init();

  tetris_game_t game;
  tetris_init(&game);
  display_render_game(&game);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_gravity_time = HAL_GetTick();
  uint32_t last_frame_time = HAL_GetTick();

  GPIO_PinState prev_left = GPIO_PIN_SET;
  GPIO_PinState prev_right = GPIO_PIN_SET;
  GPIO_PinState prev_rotate = GPIO_PIN_SET;
  GPIO_PinState prev_user_btn = GPIO_PIN_SET;
  GPIO_PinState prev_drop = GPIO_PIN_SET;

  while (1)
  {
    uint32_t now = HAL_GetTick();

    // 1. Read Button Inputs (Active-Low: 0 = Pressed, 1 = Released)
    GPIO_PinState curr_left   = HAL_GPIO_ReadPin(Btn_Left_GPIO_Port, Btn_Left_Pin);
    GPIO_PinState curr_right  = HAL_GPIO_ReadPin(Btn_Right_GPIO_Port, Btn_Right_Pin);
    GPIO_PinState curr_rotate = HAL_GPIO_ReadPin(Btn_Rotate_GPIO_Port, Btn_Rotate_Pin);
    GPIO_PinState curr_drop   = HAL_GPIO_ReadPin(Btn_Drop_GPIO_Port, Btn_Drop_Pin);
    GPIO_PinState curr_user   = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

    tetris_input_t input = TETRIS_INPUT_NONE;

    if (prev_left == GPIO_PIN_SET && curr_left == GPIO_PIN_RESET) {
      input |= TETRIS_INPUT_LEFT;
      printf("[INPUT] Left\r\n");
    }
    if (prev_right == GPIO_PIN_SET && curr_right == GPIO_PIN_RESET) {
      input |= TETRIS_INPUT_RIGHT;
      printf("[INPUT] Right\r\n");
    }
    if ((prev_rotate == GPIO_PIN_SET && curr_rotate == GPIO_PIN_RESET) ||
        (prev_user_btn == GPIO_PIN_SET && curr_user == GPIO_PIN_RESET)) {
      input |= TETRIS_INPUT_ROTATE;
      printf("[INPUT] Rotate\r\n");
    }
    if (prev_drop == GPIO_PIN_SET && curr_drop == GPIO_PIN_RESET) {
      input |= TETRIS_INPUT_DROP;
      printf("[INPUT] Drop\r\n");
    }

    prev_left     = curr_left;
    prev_right    = curr_right;
    prev_rotate   = curr_rotate;
    prev_user_btn = curr_user;
    prev_drop     = curr_drop;

    // If game over and any button pressed, restart
    if (game.state == TETRIS_STATE_GAME_OVER && input != TETRIS_INPUT_NONE) {
      tetris_init(&game);
      display_render_game(&game);
      printf("[GAME] Restarted!\r\n");
      input = TETRIS_INPUT_NONE;
    }

    // 2. Gravity Tick Calculation (speeds up with level)
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

    // 3. Step Game Simulation
    if (input != TETRIS_INPUT_NONE || gravity_tick) {
      tetris_step(&game, input, gravity_tick);
    }

    // 4. Render Frame at ~40 FPS (every 25 ms)
    if (now - last_frame_time >= 25) {
      last_frame_time = now;
      display_render_game(&game);
    }

    HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==USER_BUTTON_PIN)
  {
    BspButtonState = BUTTON_PRESSED;
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
