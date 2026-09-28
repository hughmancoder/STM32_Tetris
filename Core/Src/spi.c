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

/* HAL SPI handles */
// SPI_HandleTypeDef hspi1;
// DMA_HandleTypeDef hdma_spi1_tx;

/* SPI1 init function */
void MX_SPI1_Init(void) {

  // Enable SPI1
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  // Configure GPIO pins.
  GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7);
  // Set to Alternate function mode (10) for SPI
  GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1);

  // Set output speed to very high(11)
  GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR7);

  // No Pull-up, Pull-down (00b) (p. 186)
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5 | GPIO_PUPDR_PUPDR7);

  // Connect pins to AF5, Alternate Function Low register for SPI according to
  // p.178 fig 19.
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5_Msk | GPIO_AFRL_AFSEL7_Msk);
  GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL5_Pos) | (5U << GPIO_AFRL_AFSEL7_Pos);

  // Ensure SPI is disabled first before writing configuration
  SPI1->CR1 &= ~SPI_CR1_SPE;

  // Set Master mode 26.7.1 SPI control register 1 (CPI_CR1), ensable software
  // slave management (SSM), Internal slave select (ISS) Set baud rate prescaler
  // (0011) or f PCLK /16
  SPI1->CR1 =
      SPI_CR1_MSTR | (SPI_CR1_BR_1 | SPI_CR1_BR_0) | SPI_CR1_SSM | SPI_CR1_SSI;

  // Enable SPI1 (p. 866)
  SPI1->CR1 |= SPI_CR1_SPE;

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
 * (To be implemented by you using registers: wait for TXE in SPI1->SR, write
 * SPI1->DR, wait for RXNE, read DR)
 */
uint8_t spi1_transmit_byte(uint8_t data) {

  // 26.7.3 SPI status register (SPI_SR)
  // wait util the transmit buffer is empty
  while (!(SPI1->SR & SPI_SR_TXE)) {
    __NOP();
  }

  SPI1->DR = data;

  // wait until receive buffer is empty
  while (!(SPI1->SR & SPI_SR_RXNE)) {
    __NOP();
  }

  // Read received byte from DR, clears RXNE flag
  return (uint8_t)(SPI1->DR);
}

/**
 * @brief Transmit a buffer of bytes via SPI1
 */
void spi1_transmit_buf(const uint8_t *data, uint16_t size) {
  for (uint16_t i = 0; i < size; i++) {
    spi1_transmit_byte(data[i]);
  }
}

/**
 * @brief  Wait until the SPI bus is idle (BSY = 0)
 *  Call this before pulling CS HIGH!
 */
void spi1_wait_idle(void) {
  while (SPI1->SR & SPI_SR_BSY) {
    __NOP();
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
