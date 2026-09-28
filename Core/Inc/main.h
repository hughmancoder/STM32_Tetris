/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Level 1 Bare-metal: Commented out STM32Cube HAL headers */
// #include "stm32f4xx_hal.h"
// #include "stm32f4xx_nucleo.h"

/* CMSIS Device & Core Headers for STM32F446RE (provides register structs like
 * GPIOA, SPI1, RCC) */
#include "core_cm4.h"
#include "stm32f446xx.h"
#include <stdbool.h>
#include <stdint.h>


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Bare-metal timing helpers (SysTick based) */
void systick_init(uint32_t sys_clk_hz);
uint32_t get_millis(void);
void delay_ms(uint32_t ms);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* Bare-metal pin numbers (bit positions 0..15) */
#define Btn_Left_Pin_Pos 0U
#define Btn_Right_Pin_Pos 1U
#define Btn_Rotate_Pin_Pos 2U
#define Btn_Drop_Pin_Pos 3U
#define CS_Pin_Pos 0U
#define DC_Pin_Pos 1U
#define RESET_Pin_Pos 2U

/* Original HAL pin definitions commented out for reference */
// #define Btn_Left_Pin GPIO_PIN_0
// #define Btn_Left_GPIO_Port GPIOC
// #define Btn_Right_Pin GPIO_PIN_1
// #define Btn_Right_GPIO_Port GPIOC
// #define Btn_Rotate_Pin GPIO_PIN_2
// #define Btn_Rotate_GPIO_Port GPIOC
// #define Btn_Drop_Pin GPIO_PIN_3
// #define Btn_Drop_GPIO_Port GPIOC
// #define USART_TX_Pin GPIO_PIN_2
// #define USART_TX_GPIO_Port GPIOA
// #define USART_RX_Pin GPIO_PIN_3
// #define USART_RX_GPIO_Port GPIOA
// #define CS_Pin GPIO_PIN_0
// #define CS_GPIO_Port GPIOB
// #define DC_Pin GPIO_PIN_1
// #define DC_GPIO_Port GPIOB
// #define RESET_Pin GPIO_PIN_2
// #define RESET_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
