/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Level 1 Bare-metal: Commented out HAL SPI handles */
// SPI_HandleTypeDef hspi1;
// DMA_HandleTypeDef hdma_spi1_tx;

/* SPI1 init function */
void MX_SPI1_Init(void)
{
  /* =========================================================================
   * Bare-Metal Implementation (To be implemented by you using CMSIS registers)
   * Refer to Reference Manual RM0390 -> Section: SPI
   *
   * Tasks:
   * 1. Enable SPI1 and GPIOA Peripheral Clocks:
   *    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
   *    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
   *
   * 2. Configure PA5 (SCK) and PA7 (MOSI) for Alternate Function 5 (SPI1):
   *    - Set MODER to Alternate Function (10) for PA5 & PA7
   *    - Set OSPEEDR to High/Very High
   *    - Set AFR[0] (AFRL) to AF5 (0101) for pin 5 and pin 7
   *
   * 3. Configure SPI1 Registers (SPI1->CR1):
   *    - Master selection (MSTR)
   *    - Baud rate prescaler (BR[2:0])
   *    - Clock polarity & phase (CPOL=0, CPHA=0 for Mode 0)
   *    - 8-bit data frame format (DFF=0)
   *    - MSB first (LSBFIRST=0)
   *    - Software slave management (SSM=1, SSI=1)
   *
   * 4. Enable SPI1:
   *    SPI1->CR1 |= SPI_CR1_SPE;
   * ========================================================================= */

  /* --- Original HAL Implementation (Commented Out) ---
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  --- End of Original HAL Implementation --- */
}

/**
 * @brief Transmit 1 byte via SPI1 and return received byte
 * (To be implemented by you using registers: wait for TXE in SPI1->SR, write SPI1->DR, wait for RXNE, read DR)
 */
uint8_t spi1_transmit_byte(uint8_t data)
{
  /* TODO: Wait for TXE (Transmit buffer empty) flag in SPI1->SR */
  /* TODO: Write data to SPI1->DR */
  /* TODO: Wait for RXNE (Receive buffer not empty) flag in SPI1->SR */
  /* TODO: Return (uint8_t)(SPI1->DR) */
  (void)data;
  return 0;
}

/**
 * @brief Transmit a buffer of bytes via SPI1
 */
void spi1_transmit_buf(const uint8_t *data, uint16_t size)
{
  for (uint16_t i = 0; i < size; i++) {
    spi1_transmit_byte(data[i]);
  }
}

/* --- Original HAL MSP Functions (Commented Out) ---
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
  ...
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
  ...
}
--- End of HAL MSP Functions --- */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

