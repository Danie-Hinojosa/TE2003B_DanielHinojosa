#ifndef USER_UART_H_
#define USER_UART_H_

#include <stdint.h>
#include <stdio.h>

void USER_USART1_Init( void );
void USER_USART1_Transmit( uint8_t *pData, uint16_t size );
void USER_USART1_Send_8bit(uint8_t Data);
uint8_t USER_USART1_Receive_8bit(void);
void USART1_IRQHandler(void);

#endif /* USER_UART_H_ */
