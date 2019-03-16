#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART_USB_Init(void);
void UART2_Init(void);
void UART_USB_Send_String_n(char *, int);
void UART_USB_Send_String(char *);
char UART_USB_Recv_Char_Blocking();

char UART2_Rx_Char_B();
char UART2_Rx_Char_NB();

#endif  // UART_H