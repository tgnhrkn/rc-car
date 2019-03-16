#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "car.h"
#include "steer.h"
#include "drive.h"
#include "display.h"
#include "control.h"
#include "carqueues.h"


// Init all tasks for the car
void CarInit() {
  
  QueueInit();
  
  // Steer_Task: Output the current control direction to the servo PWM
  if(SteerTaskInit() != 0) {
    while(1) {}
  } 
  
  // Drive_Task: Output the current drive speed to the motor PWM
  if(DriveTaskInit() != 0) {
    while(1) {}
  }
  
  // Control_Task: Recieve commands from the device
  if(ControlTaskInit() != 0) {
    while(1) {}
  }
  
  // Display_Task: Control the car and/or display info on LCD
  if(DisplayTaskInit() != 0) {
    while(1) {}
  }
}