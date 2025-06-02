#include <stdint.h>
#include "user_tim.h"
#include "main.h"
#include "user_uart.h"

void USER_TIM3_PWM_Init( void ){
	/* STEP 0 Enable the clock signal for the TIM3 and GPIOB peripherals */
	RCC->IOPENR  |= (0x1UL << 1);
	RCC->APBENR1 |= (0x1UL << 1);

	// PB4 (CH1 - AF1)
	GPIOB->AFRL &= ~(0xEUL << 16U);
	GPIOB->AFRL |=  (0x1UL << 16U);
	GPIOB->PUPDR &= ~(0x3UL << 8U);
	GPIOB->OTYPER &= ~(1U << 4U);
	GPIOB->MODER &= ~(0x1UL << 8U);
	GPIOB->MODER |=  (0x2UL << 8U);

	// PB5 (CH2 - AF1)
	GPIOB->AFRL &= ~( 0xEUL << 20U );
	GPIOB->AFRL |=  ( 0x1UL << 20U );
	GPIOB->PUPDR  &= ~( 0x3UL <<  10U );
	GPIOB->OTYPER	&= ~( 0x1UL <<  5U );
	GPIOB->MODER  &= ~( 0x1UL <<  10U );
	GPIOB->MODER  |=  ( 0x2UL <<  10U );

	// PB0 (CH3 - AF3)
	GPIOB->AFRL &= ~( 0xEUL << 0U );
	GPIOB->AFRL |=  ( 0x1UL << 0U);
	GPIOB->PUPDR  &= ~( 0x3UL << 0U);
	GPIOB->OTYPER	&= ~( 0x1UL << 0U);
	GPIOB->MODER  &= ~( 0x1UL << 0U);
	GPIOB->MODER  |=  ( 0x2UL << 0U);

	// PB1 (CH4 - AF3)
	GPIOB->AFRL &= ~(0xEUL << 4U);
	GPIOB->AFRL |=  (0x1UL << 4U);
	GPIOB->PUPDR  &= ~(0x3UL << 2U);
	GPIOB->OTYPER	&= ~(0x1UL << 1U);
	GPIOB->MODER  &= ~(0x1UL << 2U);
	GPIOB->MODER  |=  (0x2UL << 2U);

	/* STEP 1. Configure the clock source (internal) */
	TIM3->SMCR	&= ~(0x1UL << 16U) &  ~(0x7UL <<  0U);

	/* STEP 2. Configure the counter mode, the auto-reload and the overflow UEV-event  */
	TIM3->CR1 &= ~(0x3UL << 5U)
			  &  ~(0x1UL << 4U)
			  &  ~(0x1UL << 2U)
			  &  ~(0x1UL << 1U);
	TIM3->CR1 |=  (0x1UL << 7U);

	/* STEP 3. Configure the prescaler, the period and the duty cycle register values */
	TIM3->PSC = 0U;
	TIM3->ARR = 47999U;
	TIM3->CCR1 = 0U;
	TIM3->CCR2 = 0U;
	TIM3->CCR3 = 0U;
	TIM3->CCR4 = 0U;

	/* STEP 4. Configure the PWM mode, the compare register load and channel direction */
	TIM3->CCMR1		&= ~( 0x1UL << 24U )
					&  ~( 0x1UL << 12U ) //		Selects PWM 1 mode for CH2
					&  ~( 0x1UL << 16U )
					&  ~( 0x1UL <<  4U ) //		Selects PWM 1 mode for CH1
					&  ~( 0x3UL <<  8U ) //		Selects CH2 as output
					&  ~( 0x3UL <<  0U );//		Selects CH1 as output
	TIM3->CCMR1		|=  ( 0x6UL << 12U ) //		Selects PWM 1 mode for CH2
					|   ( 0x1UL << 11U ) //		CCR2 loads on the UEV event
					|   ( 0x6UL <<  4U ) //		Selects PWM 1 mode for CH1
					|   ( 0x1UL <<  3U );//		CCR1 loads on the UEV event
	TIM3->CCMR2		&= ~( 0x1UL << 24U )
					&  ~( 0x1UL << 12U ) //		Selects PWM 1 mode for CH4
					&  ~( 0x1UL << 16U )
					&  ~( 0x1UL <<  4U ) //		Selects PWM 1 mode for CH3
					&  ~( 0x3UL <<  8U ) //		Selects CH4 as output
					&  ~( 0x3UL <<  0U );//		Selects CH3 as output
	TIM3->CCMR2		|=  ( 0x6UL << 12U ) //		Selects PWM 1 mode for CH4
					|   ( 0x1UL << 11U ) //		CCR4 loads on the UEV event
					|   ( 0x6UL <<  4U ) //		Selects PWM 1 mode for CH3
					|   ( 0x1UL <<  3U );//		CCR3 loads on the UEV event

	/* STEP 5. Generate the UEV-event to load the registers */
	TIM3->EGR |=  ( 0x1UL <<  0U );

	/* STEP 6. Enable the PWM signal output and set the polarity */
	TIM3->CCER		&= ~( 0x1UL << 15U ) //		for CH4 output mode, this bit must be cleared
					&  ~( 0x1UL << 13U ) //		OC4 active high
					&  ~( 0x1UL << 11U ) //		for CH3 output mode, this bit must be cleared
					&  ~( 0x1UL <<  9U ) //		OC3 active high
					&  ~( 0x1UL <<  7U ) //		for CH2 output mode, this bit must be cleared
					&  ~( 0x1UL <<  5U ) //		OC2 active high
					&  ~( 0x1UL <<  3U ) //		for CH1 output mode, this bit must be cleared
					&  ~( 0x1UL <<  1U );//		OC1 active high
	TIM3->CCER		|=  ( 0x1UL << 12U ) //		OC4 signal is output on the corresponding pin
					|   ( 0x1UL <<  8U )//		OC3 signal is output on the corresponding pin
					|   ( 0x1UL <<  4U )//		OC2 signal is output on the corresponding pin
					|   ( 0x1UL <<  0U );//		OC1 signal is output on the corresponding pin

	/* STEP 7. Enable the Timer to start counting */
	TIM3->CR1 |=  ( 0x1UL <<  0U );
}

void USER_TIM14_Init(void) {
	RCC->APBENR2 |=  (0x1UL <<  15U);
	TIM14->SMCR	 &= ~( 0x1UL << 16U)
				 &  ~( 0x7UL << 0U);
	TIM14->CR1	 &= ~( 0x1UL << 7U)
				 &  ~( 0x3UL << 5U)
				 &  ~( 0x1UL << 4U)
				 &  ~( 0x1UL << 1U);
}

void USER_TIM14_Delay(uint16_t ms) {
	TIM14->CR1 &= ~(1UL << 0);

	TIM14->PSC = 4799;
	TIM14->ARR = ms * 100;
	TIM14->EGR |= (1UL << 0);

	TIM14->SR &= ~(1UL << 0);

	TIM14->CR1 |= (1UL << 0);

	while (!(TIM14->SR & (1UL << 0)));

	TIM14->CR1 &= ~(1UL << 0);
	TIM14->SR &= ~(1UL << 0);
}

void USER_TIM16_Init(void) {
    // 1. Habilitar el reloj de TIM16 (APBENR2 bit 17)
    RCC->APBENR2 |= (0x1UL << 17U);

    // 2. Configuración básica del timer
    TIM16->SMCR &= ~(0x1UL << 16U);
    TIM16->SMCR &= ~(0x7UL << 0U);

    TIM16->CR1 &= ~(0x1UL << 7U);
    TIM16->CR1 &= ~(0x3UL << 5U);
    TIM16->CR1 &= ~(0x1UL << 4U);
    TIM16->CR1 &= ~(0x1UL << 1U);

    // 3. Configurar prescaler y periodo para el intervalo deseado
    TIM16->PSC = 4799;
    TIM16->ARR = 99;

    // 4. Limpiar la bandera UIF antes de habilitar interrupción
	TIM16->SR &= ~(1UL << 0);

	// 5. Habilitar la interrupción de "update" (UIE)
	TIM16->DIER |= (1UL << 0);

	NVIC_ISER0 = (1UL << 21);

	// 7. Arrancar el timer
	TIM16->CR1 |= (1UL << 0);
}



void USER_TIM17_Init_Timer(void) {
    RCC->APBENR2 |= (1UL << 18U);   // Enable TIM17 clock
    TIM17->CR1   &= ~(1UL << 7U);   // Auto-reload not buffered
    TIM17->CR1   &= ~(1UL << 1U);   // UEV enabled
    TIM17->PSC    = 1499U;          // Prescaler -> 1 tick = 32 µs (approx)
    TIM17->ARR    = 65535U;         // Max count
    TIM17->SR    &= ~(1UL << 0U);   // Clear overflow flag
    TIM17->CR1   |= (1UL << 0U);    // Enable counter
}

uint16_t USER_Duty_Cycle( uint8_t duty ){
	if( duty <= 100 )
		return (( duty / 100.0 )*( TIM3->ARR + 1));
	else
		return 0;
}

void USER_Set_PWM_Duty(uint8_t duty_percent) {
	uint16_t duty = USER_Duty_Cycle(duty_percent);
	TIM3->CCR1 = duty;
	TIM3->CCR2 = duty;
	TIM3->CCR3 = duty;
	TIM3->CCR4 = duty;

	TIM3->EGR |= (1U << 0);
	TIM3->CR1 &= ~(0x1UL << 1U);
}

void USER_Change_PWM_Duty(int8_t delta_percent) {
	uint16_t current_duty_ticks = TIM3->CCR1;
	uint16_t arr_value = TIM3->ARR + 1;
	float current_percent = ((float)current_duty_ticks / arr_value) * 100.0f;

	float new_percent = current_percent + delta_percent;
	if (new_percent > 100.0f) new_percent = 100.0f;
	if (new_percent < 0.0f) new_percent = 0.0f;

	uint16_t new_duty = (uint16_t)((new_percent / 100.0f) * arr_value);
	TIM3->CCR1 = new_duty;
	TIM3->CCR2 = new_duty;
	TIM3->CCR3 = new_duty;
	TIM3->CCR4 = new_duty;
}

void TIM16_IRQHandler(void) {
    if (TIM16->SR & (1UL << 0)) {
        TIM16->SR &= ~(1UL << 0);
        tim16_tick++;
    }
}

void Update_PWM_From_Velocity(int velocity_percent) {
    if (velocity_percent < 0) velocity_percent = 0;
    if (velocity_percent > 100) velocity_percent = 100;

    uint16_t duty = (velocity_percent * (TIM3->ARR + 1)) / 100;

    TIM3->CCR1 = duty;
    TIM3->CCR2 = duty;
    TIM3->CCR3 = duty;
    TIM3->CCR4 = duty;
}

