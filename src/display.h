#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>

#define NUM_MATRICES 4
void MAX7219_SPI_Init(void);
void max7219_init_all(uint8_t address, uint8_t data);
void max7219_write_row(uint8_t address, uint8_t d3, uint8_t d2, uint8_t d1,
                       uint8_t d0);
#endif // DISPLAY_H
