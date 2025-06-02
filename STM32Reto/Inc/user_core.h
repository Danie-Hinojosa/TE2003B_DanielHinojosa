#ifndef USER_CORE_H_
#define USER_CORE_H_

#include <stdint.h>

uint8_t delay_elapsed(uint32_t *start, uint32_t n_ticks);

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

#endif /* USER_CORE_H_ */
