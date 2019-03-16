#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <assert.h>
#include "pwm.h"
#include <lib_shared.h>

void PWM0_Init()
{
  SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0;  // Enable PWM module 0
  while(!(SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0));
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Enable GPIOB
  while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1));
  
  GPIO_PORTB_AFSEL_R |= 1 << 6;                 // Alternate function for pin 6
  GPIO_PORTB_AFSEL_R |= 1 << 7;                 // Alternate function for pin 7
  GPIO_PORTB_DEN_R |= 1 << 6;
  GPIO_PORTB_DEN_R |= 1 << 7;
  GPIO_PORTB_DIR_R &= ~((1 << 6) | (1 << 7)); 
  GPIO_PORTB_PCTL_R &= ~GPIO_PCTL_PB6_M;        // Clear mux pin 6
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB6_M0PWM0;    // Select Module 0, PWM 0
  GPIO_PORTB_PCTL_R &= ~GPIO_PCTL_PB7_M;        // Clear mux pin 7
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB7_M0PWM1;    // Select module 0, pwm1
  
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;         // Use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;         // Clear divider value
  SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_16;         // Set division to 16
  
  PWM0_0_CTL_R = 0;     // No sync, countdown
  PWM0_0_GENA_R = 0;
  PWM0_0_GENA_R |= PWM_0_GENA_ACTLOAD_ONE | PWM_0_GENA_ACTCMPAD_ZERO;
  
  PWM0_0_GENB_R = 0;
  PWM0_0_GENB_R |= PWM_0_GENA_ACTLOAD_ONE | PWM_0_GENA_ACTCMPAD_ZERO;
  
  PWM0_0_LOAD_R = 20000 - 1;        // 16MHz/16 = 1Mhz. 20000 Cycles = 20ms
  PWM0_0_CMPA_R = 18500;            // 18500 = 1.5ms duty
  PWM0_0_CMPB_R = 0;
  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;     // Turn on Module
  PWM0_ENABLE_R |= PWM_ENABLE_PWM0EN | PWM_ENABLE_PWM1EN;       // Enable signals 
}

void PWM0_G3_Init()
{
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;  // Enable PWM module 0
  while(!(SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0));
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // Enable GPIOC
  while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2));
  
  GPIO_PORTC_AFSEL_R |= BIT(4) | BIT(5);                 // Alternate function for pins 4/5
  GPIO_PORTC_DEN_R |= BIT(4) | BIT(5);
  GPIO_PORTC_DIR_R &= ~(BIT(4) | BIT(5));
  
  GPIO_PORTC_PCTL_R &= ~GPIO_PCTL_PC4_M;        // Clear mux pin 4
  GPIO_PORTC_PCTL_R |= GPIO_PCTL_PC4_M0PWM6;    // Select Module 0, PWM 6
  GPIO_PORTC_PCTL_R &= ~GPIO_PCTL_PC5_M;        // Clear Mux Pin 5
  GPIO_PORTC_PCTL_R |= GPIO_PCTL_PC5_M0PWM7;    // Select Module 0, PWM 7
  
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;         // Use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;         // Clear divider value
  SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_16;         // Set division to 16 (16MHz clk)
  
  PWM0_3_CTL_R = 0;     // No sync, countdown
  PWM0_3_GENA_R = 0;
  PWM0_3_GENA_R |= PWM_0_GENA_ACTLOAD_ONE | PWM_0_GENA_ACTCMPAD_ZERO;
  PWM0_3_GENB_R = 0;
  PWM0_3_GENB_R |= PWM_0_GENB_ACTLOAD_ONE | PWM_0_GENA_ACTCMPBD_ZERO;
  
  PWM0_3_LOAD_R = 20000 - 1;        // 16MHz/16 = 1Mhz. 20000 Cycles = 20ms
  PWM0_3_CMPA_R = 19000;            // 19000 = 1ms duty = lowest signal value
  PWM0_3_CMPB_R = 19000;            // 19000 = 1ms duty
  PWM0_3_CTL_R |= PWM_3_CTL_ENABLE;     // Turn on Module
  PWM0_ENABLE_R |= PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN;       // Enable signals
}

void PWM0_G3_A_Set(uint32_t val) {
    PWM0_3_CMPA_R = val;
}

void PWM0_G3_B_Set(uint32_t val) {
    PWM0_3_CMPB_R = val;
}

void PWM0_Set(uint32_t val) 
{
    PWM0_0_CMPA_R = val;
}