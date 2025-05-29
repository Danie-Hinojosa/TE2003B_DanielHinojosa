#include "main.h"
#include "user_uart.h"

#define RX_BUFFER_SIZE 64

static void USART1_SendByte(uint8_t byte);
char rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_index = 0;

void USER_USART1_Init(void) {
    // Activar reloj de GPIOA y USART1
    RCC->IOPENR  |= (1 << 0);   // GPIOAEN
    RCC->APBENR2 |= (1 << 14);  // USART1EN

    // Configurar PA9 (TX) y PA10 (RX) en modo alterno AF1
    GPIOA->AFRH &= ~((0xF << 4) | (0xF << 8));
    GPIOA->AFRH |=  (0x1 << 4) | (0x1 << 8);

    GPIOA->PUPDR &= ~((0x3 << 18) | (0x3 << 20));
    GPIOA->OTYPER &= ~((1 << 9) | (1 << 10));

    GPIOA->MODER &= ~((0x3 << 18) | (0x3 << 20));
    GPIOA->MODER |=  (0x2 << 18) | (0x2 << 20);

    // Configuración USART1: 8 bits, 1 stop bit, baudrate
    USART1->CR1 &= ~((1 << 28) | (1 << 12));
    USART1->CR2 &= ~(0x3 << 12);
    USART1->BRR  = 417;  // Para 115200 baudios @ 48 MHz

    // Habilitar USART, transmisión y recepción
    USART1->CR1 |= (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5);

    NVIC_ISER0 = (1UL << 27);
}

void USER_USART1_Send_8bit(uint8_t Data) {
	while (!( USART1->ISR & (0x1UL << 7U)))
		; // wait until next data can be written
	USART1->TDR = Data; // Data to send
}

void USER_USART1_Transmit(uint8_t *pData, uint16_t size) {
	for (int i = 0; i < size; i++) {
		USER_USART1_Send_8bit(*pData++);
	}
}

uint8_t USER_USART1_Receive_8bit(void) {
	while (!( USART1->ISR & (1UL << 5U)))
		; // Espera hasta que RXNE esté en 1 (dato recibido)
	return (uint8_t) (USART1->RDR & 0xFF); // Lee el dato recibido
}

void USART1_IRQHandler(void) {
    if (USART1->ISR & (1UL << 5U)) { // RXNE
        uint8_t received = (uint8_t)(USART1->RDR & 0xFF);

        if(received == 'I') {
            rx_index = 0;
            memset(rx_buffer, 0, RX_BUFFER_SIZE);
            rx_buffer[rx_index++] = received;
            return;
        }

        if (rx_buffer[0] == 'I') {
            if(received == 'E') {
                rx_buffer[rx_index] = '\0';

                int rpm_i1, rpm_i2, vl_i1, vl_i2, gear_i;
                if (sscanf(&rx_buffer[1], "%d,%d,%d,%d,%d,", &rpm_i1, &rpm_i2, &vl_i1, &vl_i2, &gear_i) == 5) {
                    rpm = rpm_i1 + (rpm_i2 / 100.0f);
                    vl = vl_i1 + (vl_i2 / 100.0f);
                    gear = (float)gear_i;
                    paqueteListo = 1;
                }
            } else if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = received;
            } else {
                rx_index = 0;
                memset(rx_buffer, 0, RX_BUFFER_SIZE);
            }
        }
    }
}

int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		while (!( USART1->ISR & (0x1UL << 7U)))
			;
		USART1->TDR = *ptr++;
	}
	return len;
}
