#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "adc.h"
#include "user_uart.h"
#include "user_core.h"
#include "lcd.h"

// Prototipos de funciones
void USER_RCC_Init(void);
void USER_GPIO_Init(void);
void update_display(int speed, int rpm, int gear);

// Buffer para recepción UART
char uart_rx_buffer[64];
uint8_t uart_rx_index = 0;

// Variables para seguimiento de estado anterior
static struct {
    int speed;
    int rpm;
    int gear;
} last_values = {-1, -1, -1};

int main(void) {
    // Inicialización de hardware
    USER_RCC_Init();
    USER_GPIO_Init();
    USER_USART1_Init();
    USER_SysTick_Init();
    USER_ADC_Init();
    LCD_Init();

    // Mensaje inicial
    LCD_Clear();
    LCD_Set_Cursor(1, 1);
    LCD_Put_Str("Iniciando...");
    SysTick_Delay(1000);
    LCD_Clear();

    // Dibujar encabezados fijos
    LCD_Set_Cursor(1, 1);
    LCD_Put_Str("VEL:");
    LCD_Set_Cursor(1, 12);
    LCD_Put_Str("km/h");
    LCD_Set_Cursor(2, 1);
    LCD_Put_Str("RPM:");
    LCD_Set_Cursor(2, 12);
    LCD_Put_Str("G:");

    while (1) {
        // Lectura de entradas
        uint8_t btn_state = (GPIOA->IDR & (1 << 5)) ? 1 : 0;
        uint16_t adc_val = USER_ADC_Read();

        // Enviar datos al ESP32
        char txbuf[32];
        snprintf(txbuf, sizeof(txbuf), "%u,%u\n", adc_val, btn_state);
        USER_USART1_Transmit((uint8_t *)txbuf, strlen(txbuf));

        // Recepción de datos del ESP32
        while (USART1->ISR & (1 << 5)) {
            char c = USART1->RDR & 0xFF;

            if (c == '\n' || uart_rx_index >= sizeof(uart_rx_buffer) - 1) {
                uart_rx_buffer[uart_rx_index] = '\0';

                // Limpiar retorno de carro si existe
                if (uart_rx_index > 0 && uart_rx_buffer[uart_rx_index - 1] == '\r') {
                    uart_rx_buffer[uart_rx_index - 1] = '\0';
                }

                // Parsear datos (formato: "SPD:xxx RPM:xxxx G:x")
                int speed, rpm, gear;
                if (sscanf(uart_rx_buffer, "SPD:%d RPM:%d G:%d", &speed, &rpm, &gear) == 3) {
                    update_display(speed, rpm, gear);
                }

                uart_rx_index = 0;
            } else {
                uart_rx_buffer[uart_rx_index++] = c;
            }
        }

        SysTick_Delay(300); // Delay reducido para mayor capacidad de respuesta
    }
}

void update_display(int speed, int rpm, int gear) {
    // Actualizar velocidad si cambió
    if (speed != last_values.speed) {
        LCD_Set_Cursor(1, 5);
        LCD_Put_Num(speed);
        last_values.speed = speed;
    }

    // Actualizar RPM si cambió
    if (rpm != last_values.rpm) {
        LCD_Set_Cursor(2, 5);
        LCD_Put_Num(rpm);
        last_values.rpm = rpm;
    }

    // Actualizar marcha si cambió
    if (gear != last_values.gear) {
        LCD_Set_Cursor(2, 14);
        LCD_Put_Num(gear);
        last_values.gear = gear;
    }
}

// Funciones de inicialización (igual que en tu versión original)
void USER_RCC_Init(void) {
    FLASH->ACR &= ~(0x7UL << 0U);
    FLASH->ACR |=  (0x1UL << 0U);
    while ((FLASH->ACR & (0x7UL << 0U)) != 0x001UL);

    RCC->CR &= ~(0x7UL << 11U);
    while (!(RCC->CR & (1 << 10U)));
    RCC->CR |= (1 << 8);  // Enable HSI

    RCC->CFGR &= ~(0x7UL << 0U);
    RCC->CFGR &= ~(1 << 11U);
}

void USER_GPIO_Init(void) {
    RCC->IOPENR |= (1 << 0);
    GPIOA->MODER &= ~(0x3UL << (5 * 2));
    GPIOA->PUPDR &= ~(0x3UL << (5 * 2));
}
