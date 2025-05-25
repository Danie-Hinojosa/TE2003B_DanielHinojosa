#ifndef USER_UART_H_
#define USER_UART_H_

#include <stdint.h>
#include <stdio.h>

#define USART1_BASE 0x40013800UL

void USER_USART1_Init(void);
uint8_t USER_USART1_Receive_8bit(void);
void USER_USART1_Transmit(uint8_t *pData, uint16_t size);
int _write(int file, char *ptr, int len);

#endif /* USER_UART_H_ */
