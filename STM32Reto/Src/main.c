#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "adc.h"
#include "lcd.h"
#include "user_uart.h"
#include "user_core.h"
#include "user_tim.h"

void USER_RCC_Init(void);
void USER_GPIO_Init(void);
void transmit_data(void);
void Update_Inputs(void);
void USER_Delay(void);

volatile uint8_t paqueteListo = 1;
volatile uint32_t tim16_tick = 0;

int vl = 0;
int rpm = 0;
int gear = 0;
int acceleration = 0;
int button_state = 0;

int main(void)
{
	//Local variables
	uint32_t start_tx = 0;

	/* Initialization of Peripherals */
	USER_RCC_Init(); 				// Set CLK to 48MHz
	USER_SysTick_Init();		//
	USER_GPIO_Init();				// Initialize push button (break)
	USER_TIM3_PWM_Init();		// Set TIM3 CH1-4 to PWM
	USER_TIM14_Init();			// Enable TIM14 for Delay
	USER_USART1_Init();			// Enable Full-Duplex UART communication
	LCD_Init();					// Initialize LCD
	USER_ADC_Init();
	USER_TIM16_Init();

	char *msg = "Prueba de UART desde STM32\n";
	USER_USART1_Transmit((uint8_t*)msg, strlen(msg));

	for (;;) {
			Update_PWM_From_Velocity(vl);  // Ajustar LED PWM basado en velocidad

			LCD_Clear();
			LCD_Set_Cursor(1, 1);
			LCD_Put_Str("Spd:");
			LCD_Put_Num(vl);

			LCD_Set_Cursor(1, 9);
			LCD_Put_Str("G:");
			LCD_Put_Num(gear);

			LCD_Set_Cursor(2, 1);
			LCD_Put_Str("RPM:");
			LCD_Put_Num(rpm);  // Mostrar velocidad

			delay_ms(10);

		if (delay_elapsed(&start_tx, 100)) {
			Update_Inputs();  // Solo se usa para transmisión de ADC y botón
			transmit_data();  // Envía ADC y botón al ESP32
		}
	}
}

void USER_RCC_Init(void) {
    // Flash latency y reloj a 48 MHz
	FLASH->ACR	&= ~( 0x6UL <<  0U );
	FLASH->ACR	|=  ( 0x1UL <<  0U );
	while(( FLASH->ACR & ( 0x7UL <<  0U )) != 0x001UL );

	RCC->CR		&= ~( 0x7UL << 11U );
	while(!( RCC->CR & ( 0x1UL << 10U )));

	RCC->CFGR	&= ~( 0x7UL <<  0U );
	RCC->CFGR	&= ~( 0x1UL << 11U );
}

void USER_GPIO_Init(void) {
	RCC->IOPENR = RCC->IOPENR | (0x1UL << 0U);

	// Configure PA3 as input w pull up
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 6U);
	GPIOA->PUPDR = GPIOA->PUPDR | (0x1UL << 6U);
	GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 6U);

	// Configure PA5 as output push pull (It comes in the LCD, so JIC)
	GPIOA->BSRR   = 0x1UL << 21U; // Reset PA5 low to turn off LED
	GPIOA->PUPDR  = GPIOA->PUPDR  & ~(0x3UL << 10U); // Clear pull-up/pull-down bits for PA5
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL <<  5U); // Clear output type bit for PA5
	GPIOA->MODER  = GPIOA->MODER  & ~(0x2UL << 10U); // Set PA5 as output
	GPIOA->MODER  = GPIOA->MODER  |  (0x1UL << 10U); // Set PA5 as output
}

void transmit_data() {
	uint8_t tx_buffer[32];

	sprintf((char*)tx_buffer, "ADC:%d BTN:%d\n", acceleration, button_state);
    USER_USART1_Transmit(tx_buffer, strlen((char*)tx_buffer));
}

void Update_Inputs(void) {
    acceleration = USER_ADC_Read();
    button_state = (GPIOA->IDR & (1UL << 3U)) ? 1 : 0;
}

