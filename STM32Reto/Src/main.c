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

volatile uint8_t paqueteListo = 0;
volatile uint32_t tim16_tick = 0;

int vl = 0;
int rpm = 0;
int gear = 0;
int acceleration = 0;
int button_state = 0;

int main(void)
{
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

	//Local variables
	uint32_t start_tx = 0;
	uint32_t start_led = 0;

	char *msg = "Prueba de UART desde STM32\n";
	USER_USART1_Transmit((uint8_t*)msg, strlen(msg));

  for(;;){
		if (paqueteListo) {
			paqueteListo = 0;
			Update_PWM_From_Velocity(vl);
			// Aquí puedes procesar los datos almacenados en rpm, vl y gear
      Update_PWM_From_Velocity(vl);
		}

		if (delay_elapsed(&start_tx, 100)) {
			Update_Inputs();
			LCD_Clear();
			LCD_Set_Cursor(1,1);
			LCD_Put_Str("Rpm: ");
			LCD_Put_Num(rpm);

			LCD_Set_Cursor(2,1);
			LCD_Put_Str("Gr:");
			LCD_Put_Num(gear);

			LCD_Set_Cursor(2,8);
			LCD_Put_Str("Sp: ");
			LCD_Put_Num(vl);
			transmit_data();
		}

		if (delay_elapsed(&start_led, 100)) {
			GPIOA->ODR ^= (0x1UL << 5U); // Toggle USER LED


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

	// Configure PA6 as input w pull up
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x2UL << 12U);
	GPIOA->PUPDR = GPIOA->PUPDR | (0x1UL << 12U);
	GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 12U);

	// Configure PA5 as output push pull (It comes in the LCD, so JIC)
	GPIOA->BSRR   = 0x1UL << 21U; // Reset PA5 low to turn off LED
	GPIOA->PUPDR  = GPIOA->PUPDR  & ~(0x3UL << 10U); // Clear pull-up/pull-down bits for PA5
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL <<  5U); // Clear output type bit for PA5
	GPIOA->MODER  = GPIOA->MODER  & ~(0x2UL << 10U); // Set PA5 as output
	GPIOA->MODER  = GPIOA->MODER  |  (0x1UL << 10U); // Set PA5 as output
}

void USER_Delay(void){
  __asm(" ldr r0, =12999");
  __asm(" again: sub r0, r0, #1");
  __asm(" cmp r0, #0");
  __asm(" bne again");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
}

void transmit_data() {
    uint8_t tx_buffer[64];

    int rpm_i = (int)(rpm * 100);
    int vl_i = (int)(vl * 100);
    int gear_i = (int)(gear);
    int acc_i1 = acceleration / 100;
    int acc_i2 = acceleration % 100;
    int btn_i = (int)(button_state);

    sprintf((char*)tx_buffer,
            "I%d.%02d,%d.%02d,%d,%d.%02d,%d,E\n",
            rpm_i / 100, rpm_i % 100,
            vl_i / 100,  vl_i % 100,
            gear_i,
            acc_i1,
            acc_i2,
            btn_i);

    printf("STM32 ENVIA: %s", tx_buffer);  // semihosting o redirección por UART

    USER_USART1_Transmit(tx_buffer, strlen((char*)tx_buffer));
}

void Update_Inputs(void) {
    acceleration = USER_ADC_Read();
    button_state = (GPIOA->IDR & (1UL << 6U)) ? 0 : 1;
}
