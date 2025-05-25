#ifndef USER_UART_H_
#define USER_UART_H_

#include <Arduino.h>

void USER_UART2_Init(void);
void USER_UART2_Transmit(uint8_t *pData, uint16_t size);

#endif /* USER_UART_H_ */
