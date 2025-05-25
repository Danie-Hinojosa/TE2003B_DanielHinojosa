#include "main.h"
#include "user_uart.h"

static void USART1_SendByte(uint8_t byte);

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
    USART1->CR1 |= (1 << 0) | (1 << 2) | (1 << 3);
}

static void USART1_SendByte(uint8_t byte) {
    while (!(USART1->ISR & (1 << 7)));  // Espera a que el TDR esté libre
    USART1->TDR = byte;
}

void USER_USART1_Transmit(uint8_t *pData, uint16_t size) {
    for (uint16_t i = 0; i < size; ++i) {
        USART1_SendByte(pData[i]);
    }
}

uint8_t USER_USART1_Receive_8bit(void) {
    while (!(USART1->ISR & (1 << 5)));  // Espera dato en RDR
    return (uint8_t)(USART1->RDR);
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; ++i) {
        while (!(USART1->ISR & (1 << 7)));
        USART1->TDR = ptr[i];
    }
    return len;
}
