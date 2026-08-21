#include "stm32f4xx.h"
#include "sos.h"
#include <stdio.h>

// Global tick counter incremented by SysTick interrupt every 1ms
static volatile uint32_t g_ms_ticks = 0;

/**
 * @brief SysTick Interrupt Handler (Fires every 1ms)
 */
void SysTick_Handler(void) {
    g_ms_ticks++;
}

/**
 * @brief Precise millisecond delay relying on SysTick register counter
 */
void delay_ms(uint32_t ms) {
    uint32_t start = g_ms_ticks;
    while ((g_ms_ticks - start) < ms) {
        __NOP();
    }
}

/**
 * @brief Initialize ARM Cortex-M SysTick timer for 1ms interrupts
 */
static void systick_init(void) {
    // STM32F4 default clock on reset is HSI (16 MHz)
    SysTick->LOAD = (16000000UL / 1000UL) - 1UL;
    SysTick->VAL  = 0UL;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief Bare-metal USART2 initialization (115200 Baud, 8N1) on PA2 (TX) and PA3 (RX)
 */
static void usart2_init(void) {
    // 1. Enable GPIOA and USART2 peripheral clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // 2. Set PA2 (TX) and PA3 (RX) to Alternate Function mode (10 in MODER)
    GPIOA->MODER &= ~((3U << (2 * 2)) | (3U << (3 * 2)));
    GPIOA->MODER |=  ((2U << (2 * 2)) | (2U << (3 * 2)));

    // 3. Connect PA2 and PA3 to AF7 (USART2) in AFR[0] register
    GPIOA->AFR[0] &= ~((0xFU << (2 * 4)) | (0xFU << (3 * 4)));
    GPIOA->AFR[0] |=  ((7U << (2 * 4))   | (7U << (3 * 4)));

    // 4. Set baud rate: 16 MHz / (16 * 115200) = 8.6805 -> Mantissa 8 (0x08), Fraction round(0.6805 * 16) = 11 (0x0B)
    USART2->BRR = 0x008B;

    // 5. Enable USART transmitter and peripheral
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

/**
 * @brief Retarget C standard library printf() to USART2 Data Register (DR)
 */
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        // Wait until Transmit Data Register Empty (TXE) flag is set in SR register
        while (!(USART2->SR & USART_SR_TXE)) {
            __NOP();
        }
        USART2->DR = (uint8_t)(ptr[i]);
    }
    return len;
}

int main(void) {
    // 1. Initialize hardware via bare-metal registers
    systick_init();
    usart2_init();
    sos_gpio_init();

    printf("\r\n=========================================\r\n");
    printf("  STM32F446 Pure Bare-Metal SOS Started  \r\n");
    printf("  Clock: 16 MHz HSI | Registers Only     \r\n");
    printf("=========================================\r\n");

    // 2. Main loop
    while (1) {
        play_sos();
    }
}
