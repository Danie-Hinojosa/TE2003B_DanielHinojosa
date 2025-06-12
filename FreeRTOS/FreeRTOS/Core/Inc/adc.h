#ifndef USER_ADC_H_
#define USER_ADC_H_

#include <stdint.h>

uint8_t USER_ADC_Calibration( void );
void USER_ADC_Init( void );
uint16_t USER_ADC_Read( void );

#endif /* USER_ADC_H_ */
