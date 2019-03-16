#include <tm4c123gh6pm.h>
#include <lib_shared.h>
#include "i2c.h"

static void I2C2_do(uint32_t MCS_val)
{
  I2C2_MICR_R = I2C_MICR_IC;                    // Clear interrupt status
  I2C2_MCS_R = MCS_val;     // Run it
  while(I2C2_MCS_R & I2C_MCS_BUSY);             // Wait for not busy
  while(!(I2C2_MRIS_R & I2C_MRIS_RIS));          // Wait for complete
  I2C2_MICR_R = I2C_MICR_IC;                    // Clear interrupt status
}

void I2C_Init() // Need Pullup resistors on signals
{
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R2;        // Enable clk for I2C 2
  while(!(SYSCTL_PRI2C_R & SYSCTL_PRI2C_R2));
  
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;     // Enable clk for Port E
  while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4));
  
  GPIO_PORTE_AFSEL_R |= (BIT(4) | BIT(5));        // Alternate function E4/5
  GPIO_PORTE_AMSEL_R &= ~(BIT(4) | BIT(5));       // Disable analog
  
  GPIO_PORTE_ODR_R |= BIT(5);                   // Open Drain on E5 (SDA)
  GPIO_PORTE_PUR_R &= ~BIT(5);
  GPIO_PORTE_PDR_R &= ~BIT(5);
  
  GPIO_PORTE_ODR_R &= ~BIT(4);
  GPIO_PORTE_PUR_R &= ~BIT(4);
  GPIO_PORTE_PDR_R &= ~BIT(4);
  
  GPIO_PORTE_DIR_R |= (BIT(4) | BIT(5));
  
  GPIO_PORTE_PCTL_R &= ~GPIO_PCTL_PE4_M;        // SCL for I2C 2
  GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE4_I2C2SCL;
  
  GPIO_PORTE_PCTL_R &= ~GPIO_PCTL_PE5_M;        // SDA for I2C 2
  GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE5_I2C2SDA;
  
  GPIO_PORTE_DEN_R |= (BIT(4) | BIT(5));
  
  I2C2_MCR_R = I2C_MCR_MFE;                    // Master function for I2C 2
  I2C2_MCLKOCNT_R = 0xFF;
  
  
  //100kbps
  I2C2_MTPR_R = 7;
  
  // Configure interrupts on PE1
  GPIO_PORTE_AFSEL_R &= ~BIT(1);
  GPIO_PORTE_AMSEL_R &= ~BIT(1);
  GPIO_PORTE_DEN_R |= BIT(1);
  GPIO_PORTE_DIR_R &= ~BIT(1);
  
  GPIO_PORTE_IS_R &= ~BIT(1);
  GPIO_PORTE_IBE_R &= ~BIT(1);
  GPIO_PORTE_IEV_R |= BIT(1);
  
  GPIO_PORTE_IM_R |= BIT(1);
  NVIC_EN0_R |= BIT(4);
}

int I2C_MPU_Reg_Write(uint8_t reg_num, uint8_t data)
{
  while(I2C2_MCS_R & I2C_MCS_BUSBSY);
  
  // Idle -> Start transfer
  // Initiate data send to MPU
  // Start bit and enable
  // Reg_addr gets sent
  I2C2_MSA_R = MPU_ADDR_LO_W;                   
  I2C2_MDR_R = I2C_MDR_DATA_M & reg_num;
  I2C2_do(I2C_MCS_START | I2C_MCS_RUN);
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;     // Error state on slave addr 
  }
  
  // Send Register Data
  I2C2_MDR_R = I2C_MDR_DATA_M & data;
  I2C2_do(I2C_MCS_STOP | I2C_MCS_RUN);
  if(I2C2_MCS_R & I2C_MCS_ERROR) { 
    return -1;     // Error state on data write
  }
  while(I2C2_MCS_R & I2C_MCS_BUSBSY);
  
  return 0;
  
  // Return to Idle
}

int I2C_MPU_Reg_Write_Burst(uint8_t reg_num, uint8_t * data, int n)
{
  if(n <= 0) return -1;
  while(I2C2_MCS_R & I2C_MCS_BUSBSY);
  
  I2C2_MSA_R = MPU_ADDR_LO_W;
  I2C2_MDR_R = I2C_MDR_DATA_M & reg_num;
  I2C2_do(I2C_MCS_START | I2C_MCS_RUN);
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;
  }
  
  for(int i = 0; i < n - 1; i++) {
    I2C2_MDR_R = I2C_MDR_DATA_M & data[i];
    I2C2_do(I2C_MCS_RUN);
    if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
      if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
        I2C2_MCS_R = I2C_MCS_STOP;
      }
      return -1;
    }
  }
  I2C2_MDR_R = I2C_MDR_DATA_M & data[n-1];
  I2C2_do(I2C_MCS_RUN | I2C_MCS_STOP);
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;
  }
  return 0;
}

int I2C_MPU_Reg_Read(uint8_t reg_num, uint8_t * data_store)
{
  I2C2_MSA_R = MPU_ADDR_LO_W;                   
  I2C2_MDR_R = I2C_MDR_DATA_M & reg_num;
  
  while(I2C2_MCS_R & I2C_MCS_BUSBSY);
  
  I2C2_do(I2C_MCS_START | I2C_MCS_RUN);
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;     // Error state on reg_num send
  }
  
  I2C2_MSA_R = MPU_ADDR_LO_R;
  I2C2_do(I2C_MCS_START | I2C_MCS_RUN);
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;     // Error state on data recv send
  }
  *data_store = I2C_MDR_DATA_M & I2C2_MDR_R;
  I2C2_MCS_R = I2C_MCS_STOP;
  
  while (I2C2_MCS_R & I2C_MCS_BUSBSY);
  return 0;
}

// return 0 on success, -1 on error
int I2C_MPU_Reg_Read_Burst(uint8_t reg_num, uint8_t * data, int n) {
  if (n <= 0) return -1;
  if(n == 1) return I2C_MPU_Reg_Read(reg_num, data);
  while (I2C2_MCS_R & I2C_MCS_BUSBSY);
  
  // Transmit Slave Addr + RA
  I2C2_MSA_R = MPU_ADDR_LO_W;
  I2C2_MDR_R = I2C_MDR_DATA_M & ((uint32_t) reg_num);
  I2C2_do(I2C_MCS_START | I2C_MCS_RUN);
  // Check for error
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;
  }
  
  int c = 0;
  // Recieve first byte out of transmit mode
  I2C2_MSA_R = MPU_ADDR_LO_R;
  I2C2_do(I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK);// Goes to recv mode after
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;
  }
  data[c] = (uint8_t) (I2C2_MDR_R & 0xFF);
  c++;
  
  while(c < n - 1) {    // Recieve with ACK from recv state
    I2C2_do(I2C_MCS_ACK | I2C_MCS_RUN);
    if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
      if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
        I2C2_MCS_R = I2C_MCS_STOP;
      }
      return -1;
    }
    data[c] = (uint8_t) (I2C2_MDR_R & 0xFF);
    c++;
  }
  
  // Recieve with NACK from recv
  I2C2_do(I2C_MCS_RUN);
  if(I2C2_MCS_R & I2C_MCS_ERROR) {              // Check for error
    if(!(I2C2_MCS_R & I2C_MCS_ARBLST)) {
      I2C2_MCS_R = I2C_MCS_STOP;
    }
    return -1;
  }
  data[c] = (uint8_t) (I2C2_MDR_R & 0xFF);
  
  I2C2_MCS_R = I2C_MCS_STOP;
  while (I2C2_MCS_R & I2C_MCS_BUSBSY);
  return 0;
}

