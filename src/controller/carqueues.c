#include "FreeRTOS.H"
#include "queue.h"
#include "carqueues.h"

QueueHandle_t drive_q;
QueueHandle_t steer_q;

void QueueInit() {
  drive_q = xQueueCreate(2, sizeof(char));
  if(drive_q == NULL) {
    while(1) {}
  }
  
  steer_q = xQueueCreate(2, sizeof(char));
  if(steer_q == NULL) {
    while(1) {}
  }
}