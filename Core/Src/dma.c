/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
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
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{
  /* =========================================================================
   * Bare-Metal DMA Configuration (Optional - to be implemented by you)
   * Refer to Reference Manual RM0390 -> Section: DMA controller (DMA)
   *
   * Tasks (for SPI1 TX on DMA2 Stream 3 Channel 3):
   * 1. Enable DMA2 clock in RCC->AHB1ENR
   * 2. Configure DMA2_Stream3->CR (Channel 3, Mem-to-Periph, 8-bit, MINC, etc.)
   * 3. Set peripheral target address: DMA2_Stream3->PAR = (uint32_t)&(SPI1->DR)
   * ========================================================================= */

  /* --- Original HAL DMA Implementation (Commented out) ---
  __HAL_RCC_DMA2_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  --- End of Original HAL DMA Implementation --- */
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

