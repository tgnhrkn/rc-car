#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void PWM0_Init();
void PWM0_G3_Init();
void PWM0_Set(uint32_t);

void PWM0_G3_A_Set(uint32_t);
void PWM0_G3_B_Set(uint32_t);

#endif  // PWM_H