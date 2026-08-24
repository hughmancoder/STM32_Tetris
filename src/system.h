#ifndef SYSTEM_H
#define SYSTEM_H
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void usart2_init(void);

#endif // SYSTEM_H
