#include "display.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

SPI_HandleTypeDef hspi1;

void MAX7219_SPI_Init(void) {
  printf("[SPI] Initializing SPI1 and GPIO for MAX7219...\r\n");
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_SPI1_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // CS Pin (PA0)
  printf("[SPI] Configuring PA0 as Chip Select (CS)\r\n");
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Idle High

  // SPI Pins (PA5 SCK, PA7 MOSI)
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // SPI Configuration
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  HAL_SPI_Init(&hspi1);
  printf("[SPI] SPI1 Initialization Complete.\r\n");
}

void max7219_init_all(uint8_t address, uint8_t data) {
  uint8_t tx_data[2] = {address, data};
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS Low
  for (int i = 0; i < NUM_MATRICES; i++) {
    HAL_SPI_Transmit(&hspi1, tx_data, 2, HAL_MAX_DELAY);
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // CS High
}

void max7219_write_row(uint8_t address, uint8_t d3, uint8_t d2, uint8_t d1,
                       uint8_t d0) {
  uint8_t payload[4] = {d3, d2, d1, d0};
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CS Low
  for (int i = 0; i < NUM_MATRICES; i++) {
    uint8_t tx_data[2] = {address, payload[i]};
    HAL_SPI_Transmit(&hspi1, tx_data, 2, HAL_MAX_DELAY);
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // CS High
}
