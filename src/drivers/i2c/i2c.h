#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void I2C_Init();
int I2C_MPU_Reg_Write(uint8_t, uint8_t);
int I2C_MPU_Reg_Read(uint8_t, uint8_t *);

int I2C_MPU_Reg_Read_Burst(uint8_t, uint8_t *, int n);
int I2C_MPU_Reg_Write_Burst(uint8_t, uint8_t *, int n);

#define MPU_ADDR_LO 0x68
#define MPU_ADDR_LO_W 0xD0
#define MPU_ADDR_LO_R 0xD1

#endif  // I2C_H