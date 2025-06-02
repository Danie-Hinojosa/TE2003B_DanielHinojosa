#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "adc.h"
#include "lcd.h"
#include "user_uart.h"
#include "user_core.h"
#include "user_tim.h"

/*====================[ Function Prototypes ]====================*/
// Hardware initialization
void USER_RCC_Init(void);
void USER_GPIO_Init(void);
void transmit_data(void);
void Update_Inputs(void);
void USER_Delay(void);

// Task definitions
void Task_Input(void);
void Task_Control(void);
void Task_Display(void);
void Task_Transmit(void);
void Task_PWM_Update(void);

void Task_ADC_Init(void);
void Task_PWM_Init(void);
void Task_Transmit_Init(void);
void Task_LCD_Init(void);

/*====================[ Global Variables ]====================*/
volatile uint8_t paqueteListo = 1;
volatile uint32_t tim16_tick = 0;

int vl = 0, rpm = 0, gear = 0, acceleration = 0, button_state = 0;

#define CYCLE_TIME 16 // Total cycle duration in ms

/*====================[ Main ]====================*/
int main(void)
{

	/* Initialization of Peripherals */
	USER_RCC_Init();          // Set CLK to 48MHz
	USER_GPIO_Init();         // Configure button and PA5
	USER_TIM14_Init();        // Delay timer
	USER_TIM16_Init();        // Periodic timing
	USER_TIM17_Init_Timer();
	Task_ADC_Init();          // ADC
	Task_PWM_Init();     // PWM outputs
	Task_Transmit_Init();
	Task_LCD_Init();               // LCD

	/* Main Loop with Time-Based Scheduling */
	for (;;) {

	    uint32_t cycle_start = tim16_tick; // Capture start time

	    Task_Input();          // 0-2 ms
		Task_Control();        // 4-6 ms
		Task_PWM_Update();     // 2-4 ms
		Task_Transmit();       // 6-11 ms
		Task_Display();        // 11-16 ms


		/* Synchronization to maintain consistent cycle duration */
		while ((tim16_tick - cycle_start) < CYCLE_TIME);
	}

}

/*====================[ Hardware Init Functions ]==z==================*/
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

	// Configurar PA1 como entrada con pull-up
	GPIOA->PUPDR &= ~(0x3UL << 2U);  // Limpiar bits PUPDR[2:3] para PA1
	GPIOA->PUPDR |=  (0x1UL << 2U);  // Activar pull-up en PA1
	GPIOA->MODER &= ~(0x3UL << 2U);  // Configurar PA1 como entrada (00)

	// Configure PA5 as output push pull (It comes in the LCD, so JIC)
	GPIOA->BSRR   = 0x1UL << 21U; // Reset PA5 low to turn off LED
	GPIOA->PUPDR  = GPIOA->PUPDR  & ~(0x3UL << 10U); // Clear pull-up/pull-down bits for PA5
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL <<  5U); // Clear output type bit for PA5
	GPIOA->MODER  = GPIOA->MODER  & ~(0x2UL << 10U); // Set PA5 as output
	GPIOA->MODER  = GPIOA->MODER  |  (0x1UL << 10U); // Set PA5 as output
}

/*====================[ Task Functions ]====================*/
void Task_Input(void) {
    acceleration = USER_ADC_Read();
    button_state = (GPIOA->IDR & (1UL << 1U)) ? 1 : 0;  // PA1
}

void Task_Control(void) {
    // Example logic — customize as needed
    vl = acceleration / 10;
    rpm = vl * 50;
    gear = (vl / 20) + 1;
}

void Task_PWM_Update(void) {
    Update_PWM_From_Velocity(vl);
}

void Task_Display(void) {
    LCD_Clear();
    LCD_Set_Cursor(1, 1);
    LCD_Put_Str("Spd:");
    LCD_Put_Num(vl);

    LCD_Set_Cursor(1, 9);
    LCD_Put_Str("G:");
    LCD_Put_Num(gear);

    LCD_Set_Cursor(2, 1);
    LCD_Put_Str("RPM:");
    LCD_Put_Num(rpm);
}

void Task_Transmit(void) {
    uint8_t tx_buffer[32];
    sprintf((char*)tx_buffer, "ADC:%d BTN:%d\n", acceleration, button_state);
    USER_USART1_Transmit(tx_buffer, strlen((char*)tx_buffer));
}

void Task_ADC_Init(void){
	USER_ADC_Init();          // ADC
}

void Task_PWM_Init(void){
	USER_TIM3_PWM_Init();     // PWM outputs
}

void Task_Transmit_Init(void){
	USER_USART1_Init();       // UART
}

void Task_LCD_Init(void){
	LCD_Init();               // LCD
}
