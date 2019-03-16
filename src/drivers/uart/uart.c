#include <tm4c123gh6pm.h>
#include "uart.h"
#include <stdint.h>
#include <lib_shared.h>

static void UART_USB_Send_Char_Blocking(uint32_t);

void UART_USB_Init()
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
  while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0));
  
  GPIO_PORTA_AFSEL_R |= BIT(1) | BIT(0);       // Alt. Func. for PA0, PA1 (this might be wrong)
  GPIO_PORTA_PCTL_R = 
    (GPIO_PORTA_PCTL_R & ~(GPIO_PCTL_PA0_M | GPIO_PCTL_PA1_M)) | (GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);
  GPIO_PORTA_DEN_R |= BIT(1) | BIT(0);
  GPIO_PORTA_AMSEL_R &= ~(BIT(1) | BIT(0));
  
  UART0_CTL_R &= ~UART_CTL_UARTEN;
  UART0_IBRD_R = 104;
  UART0_FBRD_R = 11;
  UART0_LCRH_R = UART_LCRH_WLEN_8;              // Length 8, 1 stop, no parity, no fifos
  
  UART0_CC_R = UART_CC_CS_PIOSC;                // Select internal clock for UART0
  
  UART0_CTL_R |= UART_CTL_UARTEN; 
}

void UART2_Init()
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2;      // UART2 Clock Enable
  while(!(SYSCTL_PRUART_R & SYSCTL_PRUART_R2));
  
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;      // PortD Clock Enable
  while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R3)); // Wait for GPIO ready
  
  GPIO_PORTD_AFSEL_R |= BIT(6) | BIT(7);        // Alt. function enable
  GPIO_PORTD_PCTL_R = 
    (GPIO_PORTD_PCTL_R & ~(GPIO_PCTL_PD6_M | GPIO_PCTL_PD7_M)) | (GPIO_PCTL_PD6_U2RX | GPIO_PCTL_PD7_U2TX);
  GPIO_PORTD_DEN_R |= BIT(6) | BIT(7);          // Digital Enable
  GPIO_PORTD_AMSEL_R &= ~(BIT(6) | BIT(7));     // Analog off
  
  UART2_CTL_R &= ~UART_CTL_UARTEN;              // UART1 Disable
  UART2_IBRD_R = 8;
  UART2_FBRD_R = 44;
  UART2_LCRH_R = 0x60;
  
  UART2_CC_R |= UART_CC_CS_PIOSC;
  
  UART2_CTL_R |= UART_CTL_UARTEN;
}

void UART_USB_Send_String(char * str)
{
  for (char * c = str; *c != '\0'; c++) {
    UART_USB_Send_Char_Blocking((uint32_t) *c);
  }
}

void UART_USB_Send_String_n(char * str, int n)
{
  for(int c = 0; c < n; c++) {
    UART_USB_Send_Char_Blocking((uint32_t) str[c]);
  }
}

static void UART_USB_Send_Char_Blocking(uint32_t c)
{
  while(UART0_FR_R & UART_FR_TXFF);
  UART0_DR_R = c;
}

char UART_USB_Recv_Char_Blocking()
{
  while(!(UART0_FR_R & UART_FR_RXFF));
  return (char) (UART0_DR_R & UART_DR_DATA_M);
}

char UART2_Rx_Char_B() {
 while(!(UART2_FR_R & UART_FR_RXFF));
 return (char) (UART2_DR_R & UART_DR_DATA_M);
}

char UART2_Rx_Char_NB() {
  if(UART2_FR_R & UART_FR_RXFF) {
    return (char) (UART2_DR_R & UART_DR_DATA_M);
  }
  return '\0';
}