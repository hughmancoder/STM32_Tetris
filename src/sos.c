#include "sos.h"
#include "stm32f4xx.h"
#include <stdio.h>

void sos_gpio_init(void) {
    // 1. Enable GPIOA peripheral clock in RCC AHB1ENR register (Bit 0)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // 2. Configure PA5 as General Purpose Output Mode (01 in MODER)
    // Clear bits [11:10] and set bit 10 to 1
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));

    // 3. Configure Output Type as Push-Pull (0 in OTYPER)
    GPIOA->OTYPER &= ~(1U << 5);

    // 4. Configure Output Speed to Low Speed (00 in OSPEEDR)
    GPIOA->OSPEEDR &= ~(3U << (5 * 2));

    // 5. Configure Pull-up/Pull-down to No Pull (00 in PUPDR)
    GPIOA->PUPDR &= ~(3U << (5 * 2));

    // Start with LED in OFF state (Bit 21 in BSRR: Reset Pin 5)
    GPIOA->BSRR = (1U << (5 + 16));
}

static void blink_led(uint32_t duration_ms) {
    // Turn LED ON using atomic Bit Set (Bit 5 in BSRR)
    GPIOA->BSRR = (1U << 5);

    // Read current pin output state directly from ODR register
    uint32_t state = (GPIOA->ODR & (1U << 5)) ? 1 : 0;
    printf("LED is ON  (PA5 State: %lu)\r\n", (unsigned long)state);

    delay_ms(duration_ms);

    // Turn LED OFF using atomic Bit Reset (Bit 21 = 5 + 16 in BSRR)
    GPIOA->BSRR = (1U << (5 + 16));

    state = (GPIOA->ODR & (1U << 5)) ? 1 : 0;
    printf("LED is OFF (PA5 State: %lu)\r\n", (unsigned long)state);

    delay_ms(250); // Pause between consecutive blinks
}

void play_sos(void) {
    int i;

    // S: 3 short blinks (200ms)
    for (i = 0; i < 3; i++) {
        blink_led(200);
    }

    delay_ms(300); // Pause between letters

    // O: 3 long blinks (600ms)
    for (i = 0; i < 3; i++) {
        blink_led(600);
    }

    delay_ms(300); // Pause between letters

    // S: 3 short blinks (200ms)
    for (i = 0; i < 3; i++) {
        blink_led(200);
    }

    // Long pause before repeating the whole SOS message
    delay_ms(2000);
}
