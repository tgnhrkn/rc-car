#include <stdint.h>
#include <lcd/SSD2119.h>
#include "display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "priorities.h"
#include "carqueues.h"

#define DISPLAYTASKSTACKSIZE 128

#define DISPLAY_STATUS_ON 1
#define DISPLAY_STATUS_OFF 0

const char LETTER_IGNITION = CONTROL_IGNITION;
const char LETTER_OFF = CONTROL_OFF;

unsigned short const Colors[16] = {
  0,                                            //0 – black                   (#000000) 	000000 	0
  ((0x00>>3)<<11) | ((0x00>>2)<<5) | (0xAA>>3),  //1 – blue                    (#0000AA) 	000001 	1
  ((0x00>>3)<<11) | ((0xAA>>2)<<5) | (0x00>>3),  //2 – green                   (#00AA00) 	000010 	2
  ((0x00>>3)<<11) | ((0xAA>>2)<<5) | (0xAA>>3),  //3 – cyan                    (#00AAAA) 	000011 	3
  ((0xAA>>3)<<11) | ((0x00>>2)<<5) | (0x00>>3),  //4 – red                     (#AA0000) 	000100 	4
  ((0xAA>>3)<<11) | ((0x00>>2)<<5) | (0xAA>>3),  //5 – magenta                 (#AA00AA) 	000101 	5
  ((0xAA>>3)<<11) | ((0x55>>2)<<5) | (0x00>>3),  //6 – brown                   (#AA5500) 	010100 	20
  ((0xAA>>3)<<11) | ((0xAA>>2)<<5) | (0xAA>>3),  //7 – white / light gray      (#AAAAAA) 	000111 	7
  ((0x55>>3)<<11) | ((0x55>>2)<<5) | (0x55>>3),  //8 – dark gray /bright black (#555555) 	111000 	56
  ((0x55>>3)<<11) | ((0x55>>2)<<5) | (0xFF>>3),  //9 – bright blue             (#5555FF) 	111001 	57
  ((0x55>>3)<<11) | ((0xFF>>2)<<5) | (0x55>>3),  //10 – bright green           (#55FF55) 	111010 	58
  ((0x55>>3)<<11) | ((0xFF>>2)<<5) | (0xFF>>3),  //11 – bright cyan            (#55FFFF) 	111011 	59
  ((0xFF>>3)<<11) | ((0x55>>2)<<5) | (0x55>>3),  //12 – bright red             (#FF5555) 	111100 	60
  ((0xFF>>3)<<11) | ((0x55>>2)<<5) | (0xFF>>3),  //13 – bright magenta         (#FF55FF) 	111101 	61
  ((0xFF>>3)<<11) | ((0xFF>>2)<<5) | (0x55>>3),  //14 – bright yellow          (#FFFF55) 	111110 	62
  ((0xFF>>3)<<11) | ((0xFF>>2)<<5) | (0xFF>>3)   //15 – bright white           (#FFFFFF) 	111111 	63
};

unsigned short fingerX;
unsigned short fingerY;

static void updateCoords();
static void draw();
static void drawButtons();
static void drawFingerCoords();

static void DisplayTask(void *pvParameters) {
  int status = DISPLAY_STATUS_OFF;
  LCD_ColorFill(Colors[12]);    // Fill red on startup
  int buttondown_state = 0; // 0 means not pressed
  while (1) {
    updateCoords();
    //draw();
    if(fingerY != 0) {  // screen pressed
      if(buttondown_state == 0) {
        buttondown_state = 1;
        if(status == DISPLAY_STATUS_OFF) {
          status = DISPLAY_STATUS_ON;
          LCD_ColorFill(Colors[10]);
          xQueueSend(steer_q, &LETTER_IGNITION, 0);
          xQueueSend(drive_q, &LETTER_IGNITION, 0);
        } else if (status == DISPLAY_STATUS_ON) {
          status = DISPLAY_STATUS_OFF;
          LCD_ColorFill(Colors[12]);
          xQueueSend(steer_q, &LETTER_OFF, 0);
          xQueueSend(drive_q, &LETTER_OFF, 0);          
        }
      }
    } else {
      if(buttondown_state != 0) {
        buttondown_state = 0;
      }
    }
  }
}

uint32_t
DisplayTaskInit(void) {
  Touch_Init();
  LCD_Init();
  
  if(xTaskCreate(DisplayTask,
                 (const portCHAR *) "Display",
                 DISPLAYTASKSTACKSIZE,
                 NULL,
                 tskIDLE_PRIORITY + PRIORITY_DISPLAY_TASK,
                 NULL) != pdTRUE)
  {
    return 1;
  }
  
  return 0;
}

void draw()
{
  drawFingerCoords();
}

void updateCoords()
{
  long coords = Touch_GetCoordsBETTER();
  fingerX = (unsigned short) ((coords >> 16) & 0xFFFF);
  fingerY = (unsigned short) (coords & 0x0000FFFF);
  
  if(fingerY >= 145 && fingerY <= 155){
    fingerY = 0;
  }
  
  if(fingerX >= 1000){
    fingerX = 0;
  } else {
    fingerX = (unsigned short) (320 - fingerX*1.5 - 70);
  }
}

// Task B.3
void drawFingerCoords()
{
  LCD_SetTextColor(255,255,255);
  LCD_SetCursor(0, 0);
  printf("xPos: %d, yPos: %d      ", fingerX, fingerY);
  
        //if stop button pressed
  if(fingerX >= 225-20  && fingerX <= 295+20 && fingerY >= 60-20 && fingerY <= 130+20){
    LCD_DrawFilledRect(225, 10, 30, 30,  Colors[12]);
  } else { //clear button area
    for(int m = 0; m < 33; m++){ //clear "lights"
      LCD_DrawLine(24, 9+m, 256, 9+m, Colors[0]);
    }
  }
}

void drawButtons() {
      //draw button
    int length = 70;
    
    LCD_DrawRect(225, 60, length, length,  Colors[12]);
    LCD_DrawRect(245, 60+10, 25, 25,  Colors[14]);
    LCD_DrawRect(245, 135-40, 25, 25,  Colors[14]);
    LCD_DrawLine(245+25, 60, 245+25, 110+25-40, Colors[0]);
    LCD_DrawLine(245, 135-40, 245, 135+25-40, Colors[0]);
}