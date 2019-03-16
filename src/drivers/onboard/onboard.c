#include "onboard.h"
#include <tm4c123gh6pm.h>
#include <stdint.h>

void onboard_led_init()
{
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;             // Enable Port F
  while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5));        // Wait for Port F ready
  GPIO_PORTF_DEN_R |= ONBOARD_LED_RBG;              // Set Port F LEDS digital
  GPIO_PORTF_DIR_R |= ONBOARD_LED_RBG;              // Set LED pins as outputs
  onboard_led_set(ONBOARD_LED_OFF);              // Reset data for good measure
}

void onboard_led_set(onboard_led_state_t set)
{
  *(GPIO_PORTF_DATA_BITS_R + ONBOARD_LED_RBG) = set;
}