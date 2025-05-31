#ifndef USER_UART_H_
#define USER_UART_H_

#include <stdint.h>
#include <stdio.h>

#define USART_CR1_UE       (1 << 0)
#define USART_CR1_RE       (1 << 2)
#define USART_CR1_TE       (1 << 3)
#define USART_CR1_RXNEIE   (1 << 5)

#define USART_ISR_RXNE     (1 << 5)
#define USART_ISR_TXE      (1 << 7)


void USER_USART1_Init( void );
void USER_USART1_Transmit( uint8_t *pData, uint16_t size );
void USER_USART1_Send_8bit(uint8_t Data);
uint8_t USER_USART1_Receive_8bit(void);
void USART1_IRQHandler(void);
void parse_and_display(const char *line);

#endif /* USER_UART_H_ */
