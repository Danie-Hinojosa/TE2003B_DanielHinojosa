#include <stdint.h>
#include "user_tim.h"
#include "main.h"
#include "user_uart.h"

void USER_TIM3_PWM_Init( void ){
	/* STEP 0 Enable the clock signal for the TIM3 and GPIOB peripherals */
	RCC->IOPENR  |= (1U << 1);
	RCC->APBENR1 |= (1U << 1);

	// PB4 (CH1 - AF1)
	GPIOB->AFRL &= ~(0xFUL << 16U);
	GPIOB->AFRL |=  (0x1UL << 16U);
	GPIOB->PUPDR &= ~(0x3UL << 8U);
	GPIOB->OTYPER &= ~(1U << 4);
	GPIOB->MODER &= ~(0x3UL << 8U);
	GPIOB->MODER |=  (0x2UL << 8U);

	// PB5 (CH2 - AF1)
	GPIOB->AFRL &= ~( 0xFUL << 20U );
	GPIOB->AFRL |=  ( 0x1UL << 20U );
	GPIOB->PUPDR  &= ~( 0x3UL <<  10U );
	GPIOB->OTYPER	&= ~( 0x1UL <<  5U );
	GPIOB->MODER  &= ~( 0x1UL <<  10U );
	GPIOB->MODER  |=  ( 0x2UL <<  10U );

	// PB6 (CH3 - AF3)
	GPIOB->AFRL &= ~( 0xFUL << 24U );
	GPIOB->AFRL |=  ( 0x3UL << 24U);
	GPIOB->PUPDR  &= ~( 0x3UL << 12U);
	GPIOB->OTYPER	&= ~( 0x1UL << 6U);
	GPIOB->MODER  &= ~( 0x1UL << 12U);
	GPIOB->MODER  |=  ( 0x2UL << 12U);

	// PB7 (CH4 - AF3)
	GPIOB->AFRL &= ~(0xFUL << 28U);
	GPIOB->AFRL |=  (0x3UL << 28U);
	GPIOB->PUPDR  &= ~(0x3UL << 14U);
	GPIOB->OTYPER	&= ~(0x1UL << 7U);
	GPIOB->MODER  &= ~(0x1UL << 14U);
	GPIOB->MODER  |=  (0x2UL << 14U);

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
	uint16_t duty = USER_Duty_Cycle(0);
	TIM3->CCR1 = duty;
	TIM3->CCR2 = duty;
	TIM3->CCR3 = duty;
	TIM3->CCR4 = duty;

	/* STEP 4. Configure the PWM mode, the compare register load and channel direction */
	/* For CH1 and CH2 -> CCMR1 */
	TIM3->CCMR1 &= ~((0x3UL << 0U) | (0x7UL << 4U) | (0x1UL << 3U));
	TIM3->CCMR1 |=  ((0x0UL << 0U) | (0x6UL << 4U) | (0x1UL << 3U));
	TIM3->CCMR1 &= ~((0x3UL << 8U) | (0x7UL << 12U) | (0x1UL << 11U));
	TIM3->CCMR1 |=  ((0x0UL << 8U) | (0x6UL << 12U) | (0x1UL << 11U));

	/* For CH3 and CH4 -> CCMR2 */
	TIM3->CCMR2 &= ~((0x3UL << 0U) | (0x7UL << 4U) | (0x1UL << 3U));
	TIM3->CCMR2 |=  ((0x0UL << 0U) | (0x6UL << 4U) | (0x1UL << 3U));
	TIM3->CCMR2 &= ~((0x3UL << 8U) | (0x7UL << 12U) | (0x1UL << 11U));
	TIM3->CCMR2 |=  ((0x0UL << 8U) | (0x6UL << 12U) | (0x1UL << 11U));

	/* STEP 5. Generate the UEV-event to load the registers */
	TIM3->EGR |=  ( 0x1UL <<  0U );

	/* STEP 6. Enable the PWM signal output and set the polarity */
	// CH1: OC1 active high, output enable
	TIM3->CCER &= ~( 0x1UL <<  3U )
			   &  ~( 0x1UL <<  1U );
	TIM3->CCER |=  ( 0x1UL <<  0U );

	// CH2: OC2 active high, output enable
	TIM3->CCER &= ~( 0x1UL <<  7U )
			   &  ~( 0x1UL <<  5U );
	TIM3->CCER |=  ( 0x1UL <<  4U );

	// CH3: OC3 active high, output enable
	TIM3->CCER &= ~( 0x1UL << 11U )
			   &  ~( 0x1UL <<  9U );
	TIM3->CCER |=  ( 0x1UL <<  8U );

	// CH4: OC4 active high, output enable
	TIM3->CCER &= ~( 0x1UL << 15U )
			   &  ~( 0x1UL << 13U );
	TIM3->CCER |=  ( 0x1UL << 12U );

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

void USER_TIM14_Delay(uint16_t prescaler, uint16_t maxCount) {
	TIM14->CR1 &= ~(1UL << 0);

	TIM14->PSC = prescaler;
	TIM14->ARR = maxCount;
	TIM14->EGR |= (1UL << 0);

	TIM14->SR &= ~(1UL << 0);

	TIM14->CR1 |= (1UL << 0);

	while (!(TIM14->SR & (1UL << 0)));

	TIM14->CR1 &= ~(1UL << 0);
	TIM14->SR &= ~(1UL << 0);
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

void Update_PWM_From_Velocity(float input_vl) {
    if (input_vl < 0.0f) input_vl = 0.0f;
    if (input_vl > 200.0f) input_vl = 200.0f;
    uint8_t duty = (uint8_t)((input_vl / 200.0f) * 100.0f);
    USER_Set_PWM_Duty(duty);
}
