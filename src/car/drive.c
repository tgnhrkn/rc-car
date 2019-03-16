#include <stdint.h>
#include "drive.h"
#include "pwm/pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "priorities.h"
#include "carqueues.h"
#include "utils/uartstdio.h"

#define DRIVETASKSTACKSIZE 128
#define DRIVE_SPEED_MAX 18800
#define DRIVE_SPEED_MIN 18500
#define DRIVE_STEP 10

#define DRIVE_STATUS_ON 1
#define DRIVE_STATUS_OFF 0

static void
DriveTask(void * pvParameters) {
  // get current value
  // send to PWM
  char recv;
  uint32_t current = DRIVE_SPEED_MAX;   // PWM signal compare value is inversely
                                        // proportional to speed
  int status = DRIVE_STATUS_OFF;
  PWM0_G3_A_Set(current);
  while(1) {
    if (xQueueReceive(drive_q, (void *) &recv, 1000) == pdTRUE) {
      switch (recv) {
      case DRIVE_SPEED_UP:
        if(status == DRIVE_STATUS_ON) {
          current -= DRIVE_STEP;
        }
        break;
      case DRIVE_SPEED_STOP:
        if(status == DRIVE_STATUS_ON) {
          current = DRIVE_SPEED_MAX;
        }
        break;
      case DRIVE_SPEED_DOWN:
        if(status == DRIVE_STATUS_ON) {
          current += DRIVE_STEP;
        }
        break;
      case CONTROL_IGNITION:
        if(status != DRIVE_STATUS_ON) {
          status = DRIVE_STATUS_ON;
        }
        break;
      case CONTROL_OFF:
        if(status != DRIVE_STATUS_OFF) {
          current = DRIVE_SPEED_MAX;
          status = DRIVE_STATUS_OFF;
        }
        break;
      default:
        while(1) {}     // Maybe replace with debug flashing
        break;
      }
      if(current > DRIVE_SPEED_MAX) current = DRIVE_SPEED_MAX;
      if(current < DRIVE_SPEED_MIN) current = DRIVE_SPEED_MIN;
      PWM0_G3_A_Set(current);
    }
  }
}

uint32_t DriveTaskInit() {
  // PWM_M1_45 Enable
  PWM0_G3_Init();
  
  // call xTaskCreate
  if(xTaskCreate(DriveTask,
                 (const portCHAR *) "Drive",
                 DRIVETASKSTACKSIZE,
                 NULL,
                 tskIDLE_PRIORITY + PRIORITY_DRIVE_TASK,
                 NULL) != pdTRUE)
  {
    return 1;
  }
  return 0;
  
}