#include "main.h"
#include "user_uart.h"

#define RX_BUFFER_SIZE 64
#define UART_RX_BUFFER_SIZE 64

void USART1_SendByte(uint8_t byte);
volatile char uart_rx_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart_rx_index = 0;

void USER_USART1_Init(void) {
    // Activar reloj de GPIOA y USART1
    RCC->IOPENR  |= (1 << 0);   // GPIOAEN
    RCC->APBENR2 |= (1 << 14);  // USART1EN

    // Configurar PA9 (TX) y PA10 (RX) en modo alterno AF1
    GPIOA->AFR[1] &= ~((0xF << 4) | (0xF << 8));
    GPIOA->AFR[1] |=  (0x1 << 4) | (0x1 << 8);

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

    NVIC->ISER[0] = (1UL << 27);
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
    if (USART1->ISR & USART_ISR_RXNE) {  // Dato recibido
        char c = USART1->RDR & 0xFF;

        if (c == '\n' || uart_rx_index >= UART_RX_BUFFER_SIZE - 1) {
            uart_rx_buffer[uart_rx_index] = '\0';  // Finalizar string
            parse_and_display((const char*)uart_rx_buffer);  // Procesar
            uart_rx_index = 0;  // Reiniciar índice
        } else {
            uart_rx_buffer[uart_rx_index++] = c;
        }
    }
}

int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		while (!( USART2->ISR & (0x1UL << 7U)))
			;
		USART2->TDR = *ptr++;
	}
	return len;
}

void parse_and_display(const char *line) {
    int spd_entero = 0, rpm_entero = 0, gear_entero = 0;

    if (sscanf(line, "SPD:%d RPM:%d G:%d", &spd_entero, &rpm_entero, &gear_entero) == 3) {
        vl = spd_entero / 10;
        rpm = rpm_entero;
        gear = gear_entero;
        paqueteListo = 1;
    }
}

void USER_UART2_Init( void ){
  RCC->IOPENR   |=  ( 0x1UL <<  0U ); // GPIOA clock enabled
  RCC->APBENR1  |=  ( 0x1UL << 17U ); // USART2 clock enabled
  GPIOA->AFR[0]   &= ~( 0xEUL <<  8U );
  GPIOA->AFR[0]   |=  ( 0x1UL <<  8U ); // Select the AF1 for the PA2
  GPIOA->PUPDR  &= ~( 0x3UL <<  4U ); // Clear pull-up/pull-down bits for PA2
  GPIOA->OTYPER &= ~( 0x1UL <<  2U ); // Clear output type bit for PA2
  GPIOA->MODER  &= ~( 0x1UL <<  4U );
  GPIOA->MODER  |=  ( 0x2UL <<  4U ); // Set PA2 as AF
  GPIOA->AFR[0]   &= ~( 0xEUL << 12U );
  GPIOA->AFR[0]   |=  ( 0x1UL << 12U ); // Select the AF1 for the PA3
  GPIOA->PUPDR  &= ~( 0x3UL <<  6U ); // Clear pull-up/pull-down bits for PA3
  GPIOA->OTYPER &= ~( 0x1UL <<  3U ); // Clear output type bit for PA3
  GPIOA->MODER  &= ~( 0x1UL <<  6U );
  GPIOA->MODER  |=  ( 0x2UL <<  6U ); // Set PA3 as AF
  USART2->CR1   &= ~( 0x1UL << 28U ); // 8-bit word length
  USART2->CR1   &= ~( 0x1UL << 12U ); // 8-bit word length
  USART2->BRR   =   ( 48000000 / 115200 ); // Desired baud rate
  USART2->CR2   &= ~( 0x3UL << 12U ); // 1 stop bit
  USART2->CR1   |=  ( 0x1UL <<  0U ); // USART is enabled
  USART2->CR1   |=  ( 0x1UL <<  3U ); // Transmitter is enabled
  USART2->CR1   |=  ( 0x1UL <<  2U ); // Receiver is enabled
}
