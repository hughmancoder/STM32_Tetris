#ifndef SOS_H
#define SOS_H

#include <stdint.h>

/**
 * @brief Initialize GPIOA Pin 5 (LD2 green LED) using bare-metal register manipulation.
 */
void sos_gpio_init(void);

/**
 * @brief Plays the SOS morse code pattern on PA5 using bare-metal register operations.
 */
void play_sos(void);

/**
 * @brief Hardware millisecond delay using SysTick registers.
 * @param ms Duration in milliseconds.
 */
void delay_ms(uint32_t ms);

#endif /* SOS_H */
