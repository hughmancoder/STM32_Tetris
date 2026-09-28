/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> USART2_TX
     PA3   ------> USART2_RX
*/
void MX_GPIO_Init(void)
{
  /* =========================================================================
   * Bare-Metal Implementation (To be implemented by you using CMSIS registers)
   * Refer to Reference Manual RM0390 -> Section: GPIO
   *
   * Tasks:
   * 1. Enable GPIO Clocks:
   *    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
   *
   * 2. Configure Output Pins for Display (GPIOB):
   *    - CS (PB0), DC (PB1), RESET (PB2)
   *    - Configure as General Purpose Output in GPIOB->MODER
   *    - Set speed to High/Very High in GPIOB->OSPEEDR
   *    - Set initial output levels using GPIOB->BSRR
   *
   * 3. Configure Input Pins for Buttons (GPIOC):
   *    - Btn_Left (PC0), Btn_Right (PC1), Btn_Rotate (PC2), Btn_Drop (PC3)
   *    - Configure as Input mode in GPIOC->MODER
   *    - Enable internal Pull-Up resistors in GPIOC->PUPDR
   * ========================================================================= */

  /* --- Original HAL Implementation (Commented Out) ---
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, CS_Pin|DC_Pin|RESET_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = Btn_Left_Pin|Btn_Right_Pin|Btn_Rotate_Pin|Btn_Drop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CS_Pin|DC_Pin|RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  --- End of Original HAL Implementation --- */
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
