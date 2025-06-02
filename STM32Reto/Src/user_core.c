#include <stdint.h>
#include "main.h"
#include "user_core.h"
#include "user_tim.h"

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 48000; j++) {
            // Ajusta 48000 según la frecuencia de tu CPU
        }
    }
}


void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        for (volatile uint32_t j = 0; j < 48; j++) {
            // Ajusta 48 según la frecuencia de tu CPU
        }
    }
}

uint8_t delay_elapsed(uint32_t *start, uint32_t n_ticks) {
    if ((tim16_tick - *start) >= n_ticks) {
        *start = tim16_tick;
        return 1;
    }
    return 0;
}

