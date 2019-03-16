#ifndef ONBOARD_H
#define ONBOARD_H

void onboard_led_init();
//void onboard_switch_init();

typedef enum {
  ONBOARD_LED_OFF = 0,
  ONBOARD_LED_R = 0x2,
  ONBOARD_LED_B = 0x4,
  ONBOARD_LED_RB = 0x6,
  ONBOARD_LED_G = 0x8,
  ONBOARD_LED_RG = 0xA,
  ONBOARD_LED_BG = 0xC,
  ONBOARD_LED_RBG = 0xE
} onboard_led_state_t;

void onboard_led_set(onboard_led_state_t);

#endif  // ONBOARD_H