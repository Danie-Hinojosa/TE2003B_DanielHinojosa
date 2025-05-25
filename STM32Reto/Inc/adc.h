#ifndef USER_ADC_H_
#define USER_ADC_H_

#include <stdint.h>

void USER_ADC_Init(void);
uint16_t USER_ADC_Read(void);
void USER_ADC_Calibration(void);

#endif /* USER_ADC_H_ */
