#include "mpu6050.h"
#include "../i2c/i2c.h"

void MPU6050_Init()
{
  
}

void MPU6050_Startup()
{
  
  
}

int MPU6050_Device_Reset()
{
  int err = I2C_MPU_Reg_Write(MPU6050_PWR_MGMT_1, 0x80);
  for(int i = 0; i < 5000000; i++); // 100ms-ish delay
  return err;
}

int MPU6050_Accel_Offset_Get(mpu6050_accel_offsets_t * data_store)
{
  int err = I2C_MPU_Reg_Read_Burst(MPU6050_XG_OFFS_USRH, (uint8_t *) data_store, 6);
  if(err) return err;
  swap_char(&(data_store->reg.x_h), &(data_store->reg.x_l));
  swap_char(&(data_store->reg.y_h), &(data_store->reg.y_l));
  swap_char(&(data_store->reg.z_h), &(data_store->reg.z_l));
  return 0;
}

int MPU6050_Accel_Offset_Set(mpu6050_accel_offsets_t * data)
{
  uint8_t reg_vals[6];
  reg_vals[0] = data->reg.x_l;
  reg_vals[1] = data->reg.x_h;
  reg_vals[2] = data->reg.y_l;
  reg_vals[3] = data->reg.y_h;
  reg_vals[4] = data->reg.z_l;
  reg_vals[5] = data->reg.z_h;
  
  return I2C_MPU_Reg_Write_Burst(MPU6050_XG_OFFS_USRH, reg_vals, 6);
}

int MPU6050_Read_Sensor_Data(mpu6050_sensor_data_t * data_store)
{
  int err = I2C_MPU_Reg_Read_Burst(MPU6050_ACCEL_XOUT_H, (uint8_t *) data_store, 14);
  if(err) return err;
  
  swap_char(&(data_store->reg.x_accel_h), &(data_store->reg.x_accel_l));
  swap_char(&(data_store->reg.y_accel_h), &(data_store->reg.y_accel_l));
  swap_char(&(data_store->reg.z_accel_h), &(data_store->reg.z_accel_l));
  
  swap_char(&(data_store->reg.t_h), &(data_store->reg.t_l));
  
  swap_char(&(data_store->reg.x_gyro_h), &(data_store->reg.x_gyro_l));
  swap_char(&(data_store->reg.y_gyro_h), &(data_store->reg.y_gyro_l));
  swap_char(&(data_store->reg.z_gyro_h), &(data_store->reg.z_gyro_l));
  
  data_store->value.x_accel += -637;
  data_store->value.y_accel += 1550;
  data_store->value.z_accel += 954;
  
  return 0;
}

int MPU6050_Enable_Data_Interrupt()
{
  return I2C_MPU_Reg_Write(MPU6050_INT_ENABLE, 0x01);
}

int MPU6050_Disable_Interrupts()
{
  return I2C_MPU_Reg_Write(MPU6050_INT_ENABLE, 0x00);
}

int MPU6050_Set_Sample_Rate(uint8_t div)
{
  return I2C_MPU_Reg_Write(MPU6050_SMPLRT_DIV, div);
}