#include <tm4c123gh6pm.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <uart/uart.h>
#include "control.h"
#include "priorities.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "lib_shared.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "carqueues.h"
#include "utils/uartstdio.h"

#define CONTROLTASKSTACKSIZE 128

static void
ControlTask(void *pvParameters) {
  
  while(1)
  {
    char c = UART2_Rx_Char_NB();
    switch (c) {
    case DRIVE_SPEED_UP:
    case DRIVE_SPEED_STOP:
    case DRIVE_SPEED_DOWN:
      xQueueSend(drive_q, &c, 0);
      break;
    case STEER_RIGHT:
    case STEER_CENTER:
    case STEER_LEFT:
      xQueueSend(steer_q, &c, 0);
      break;
    case CONTROL_IGNITION:
    case CONTROL_OFF:
      // TODO
      break;
    default:
      UARTprintf("No command \r\n"); 
      break;
    }
  }
}

uint32_t
ControlTaskInit(void) {
  
  UART2_Init();
  
  // Call xTaskCreate
  if(xTaskCreate(ControlTask, 
                 (const portCHAR *) "Control",
                 CONTROLTASKSTACKSIZE, 
                 NULL, 
                 tskIDLE_PRIORITY + PRIORITY_CONTROL_TASK,
                 NULL) != pdTRUE)
  {
    return 1;
  }
  
  return 0;      
}

