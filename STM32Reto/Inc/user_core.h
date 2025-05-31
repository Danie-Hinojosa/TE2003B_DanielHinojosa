#ifndef USER_CORE_H_
#define USER_CORE_H_

#include <stdint.h>

/* System Time registers */
typedef struct
{
	volatile uint32_t CSR;
	volatile uint32_t RVR;
	volatile uint32_t CVR;
	volatile uint32_t CALIB;
} Systick_TypeDef;

#define Systick_BASE  0xE000E010UL//	Systick base address
#define Systick    	  (( Systick_TypeDef * )Systick_BASE )

void USER_SysTick_Init( void );
void SysTick_Delay( uint32_t time );
uint8_t delay_elapsed(uint32_t *start, uint32_t n_ticks);

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

#endif /* USER_CORE_H_ */
