#include <stdint.h>
#include "steer.h"
#include "pwm/pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "priorities.h"
#include "carqueues.h"
#include "utils/uartstdio.h"

#define STEERTASKSTACKSIZE 128

#define STEER_LEFT_PWM 18750
#define STEER_CENTER_PWM 18500
#define STEER_RIGHT_PWM 18250

#define STEER_STATUS_ON 1
#define STEER_STATUS_OFF 0

static void
SteerTask(void * pvParameters) {
  // get current value
  // send to PWM
  char recv;
  PWM0_G3_B_Set(STEER_CENTER_PWM);
  int status = STEER_STATUS_OFF;
  while(1) {
    if (xQueueReceive(steer_q, (void *) &recv, 1000) == pdTRUE) {
      switch (recv) {
      case STEER_LEFT:
        if(status == STEER_STATUS_ON) {
          PWM0_G3_B_Set(STEER_LEFT_PWM);
        }
        break;
      case STEER_CENTER:
        if(status == STEER_STATUS_ON) {
          PWM0_G3_B_Set(STEER_CENTER_PWM);
        }
        break;
      case STEER_RIGHT:
        if(status == STEER_STATUS_ON) {
          PWM0_G3_B_Set(STEER_RIGHT_PWM);
        }
        break;
      case CONTROL_IGNITION:
        if (status != STEER_STATUS_ON) {
          status = STEER_STATUS_ON;
        }
        break;
      case CONTROL_OFF:
        if (status != STEER_STATUS_OFF) {
          PWM0_G3_B_Set(STEER_CENTER_PWM);
          status = STEER_STATUS_OFF;
        }
        break;
      default:
        while(1) {}     // Maybe replace with debug flashing
        break;
      }
    }
  }
}

uint32_t SteerTaskInit() {
  // PWM_M1_45 Enable
  PWM0_G3_Init();
  
  // call xTaskCreate
  if(xTaskCreate(SteerTask,
                 (const portCHAR *) "Drive",
                 STEERTASKSTACKSIZE,
                 NULL,
                 tskIDLE_PRIORITY + PRIORITY_STEER_TASK,
                 NULL) != pdTRUE)
  {
    return 1;
  }
  return 0;
  
}