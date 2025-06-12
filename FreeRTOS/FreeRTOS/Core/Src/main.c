/* **************** START *********************** */
/* Libraries, Definitions and Global Declarations */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "user_uart.h"
#include "user_tim.h"
#include "adc.h"
#include "lcd.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define configUSE_PREEMPTION    1

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;
TaskHandle_t Task4Handle;
TaskHandle_t Task5Handle;

/*====================[ Function Prototypes ]====================*/
// Hardware initialization
void USER_RCC_Init(void);
void USER_GPIO_Init(void);
void transmit_data(void);
void Update_Inputs(void);
void USER_Delay(void);
void Task_ADC_Init(void);

/*====================[ Task Prototypes ]====================*/
void Task_Input(void *pvParameters);
void Task_Control(void *pvParameters);
void Task_Display(void *pvParameters);
void Task_Transmit(void *pvParameters);
void Task_PWM_Update(void *pvParameters);


/*====================[ Global Variables ]====================*/
volatile uint8_t paqueteListo = 1;
volatile uint32_t tim16_tick = 0;

int vl = 0, rpm = 0, gear = 0, acceleration = 0, button_state = 0;

#define CYCLE_TIME 16 // Total cycle duration in ms

uint8_t m[]="UART inicializando\r\n";

/*====================[ Main ]====================*/
int main(void) {
    /* Initialization of Peripherals */

	HAL_Init();
	USER_RCC_Init();
	USER_GPIO_Init();
	USER_TIM14_Init();
	USER_TIM16_Init();
	USER_TIM17_Init_Timer();
	USER_USART1_Init();
	USER_UART2_Init( );

	Task_ADC_Init();  // <- esto es nuevo y necesario
	USER_TIM3_PWM_Init();
	LCD_Init();


    USER_USART1_Transmit(m,sizeof(m));

    /* Create FreeRTOS Tasks */
	xTaskCreate(Task_PWM_Update, "PWM_Update", 256, NULL, 4, &Task3Handle);
	xTaskCreate(Task_Transmit, "Transmit", 256, NULL, 3, &Task4Handle);
	xTaskCreate(Task_Display, "Display", 128, NULL, 5, &Task5Handle);
	xTaskCreate(Task_Control, "Control", 256, NULL, 2, &Task2Handle);
	xTaskCreate(Task_Input, "Input", 128, NULL, 1, &Task1Handle);

	printf("Heap: %u bytes\r\n", xPortGetFreeHeapSize());


    vTaskStartScheduler();

	/* Repetitive block */

    for(;;){

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
	SystemCoreClock = 48000000;
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

/*====================[ FreeRTOS Task Definitions ]====================*/
void Task_Input(void *pvParameters) {
	for (;;) {
		acceleration = USER_ADC_Read();
		button_state = (GPIOA->IDR & (1UL << 1U)) ? 0 : 1;
		printf("T1\r\n");
		vTaskDelay(40);  // Delay for stability
	}
}

void Task_Control(void *pvParameters) {
	for (;;) {
		if (!paqueteListo) {
			// Solo si no llegó dato de ESP32, actualiza desde el potenciómetro
		    vl = acceleration / 10;
		    rpm = vl * 50;
		    gear = (vl / 20) + 1;
		}
		printf("T2\r\n");
		vTaskDelay(60);  // Execute periodically
	}
}

void Task_PWM_Update(void *pvParameters) {
    for (;;) {
        Update_PWM_From_Velocity(vl);
        paqueteListo = 0;
        printf("T3\r\n");
        vTaskDelay(240); // Prevent 100% CPU usage
    }
}

void Task_Display(void *pvParameters) {
    for (;;) {
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
        printf("T5\r\n");
        vTaskDelay(320);
    }
}


void Task_Transmit(void *pvParameters) {
    for (;;) {
        uint8_t tx_buffer[32];
        sprintf((char*)tx_buffer, "ADC:%d BTN:%d\r\n", acceleration, button_state);
        USER_USART1_Transmit(tx_buffer, strlen((char*)tx_buffer));
        printf("T4\r\n");
        vTaskDelay(120);
    }
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

