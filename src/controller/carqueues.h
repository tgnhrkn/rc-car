#ifndef CARQUEUES_H
#define CARQUEUES_H

#define DRIVE_SPEED_UP 'U'
#define DRIVE_SPEED_STOP 'S'
#define DRIVE_SPEED_DOWN 'D'
#define STEER_RIGHT 'R'
#define STEER_CENTER 'C'
#define STEER_LEFT 'L'
#define CONTROL_IGNITION 'I'
#define CONTROL_OFF 'O'

void QueueInit(void);

extern QueueHandle_t drive_q;
extern QueueHandle_t steer_q;

#endif  // CARQUEUES_H