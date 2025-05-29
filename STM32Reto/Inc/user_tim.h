#ifndef USER_TIM_H_
#define USER_TIM_H_

#include "main.h"

void USER_TIM3_PWM_Init(void);
uint16_t USER_Duty_Cycle(uint8_t duty);
void USER_Set_PWM_Duty(uint8_t duty_percent);
void USER_TIM14_Init(void);
void USER_TIM14_Delay(uint16_t prescaler, uint16_t AutoReload);
void USER_TIM16_Init(void);
void TIM16_IRQHandler(void);
void Update_PWM_From_Velocity(float);

#endif /* USER_TIM_H_ */
