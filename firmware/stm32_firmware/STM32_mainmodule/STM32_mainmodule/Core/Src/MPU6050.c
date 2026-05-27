#include <MPU6050.h>
#include "stdio.h"
#include "string.h"
#include "math.h"

int enable_MPU6050 = 1;

int MPU6050_init(void)
{
	if (enable_MPU6050 == 0) {
		return 0;
	}

	uint8_t check;
	uint8_t Data;

	//WHO_AM_I check device ID
	HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);  // read WHO_AM_I
	if (check != 0x68)  // if device is not present, return error code
	{
	  return -1;
	}

	// power management register 0X6B we should write all 0's to wake the sensor up
	Data = 0;
	HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, 1000);

	// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
	Data = 0x07;
	HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

	//set DLPF_CFG = 0
	Data = 0x00;
	HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, CONFIG_REG, 1, &Data, 1, 1000);

	// Set accelerometer configuration in ACCEL_CONFIG Register
	Data = 0x00;  // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> <strong>±</strong> 2g
	HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

	// Set Gyroscopic configuration in GYRO_CONFIG Register
	Data = 0x00;  // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> <strong>±</strong> 250 ̐/s
	HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);

	return 0;
}

int MPU6050_close(void)
{
	if (enable_MPU6050 == 0) {
		return 0;
	}

	return 0;
}

int MPU6050_Read_Accel (MPU6050_Data_t *result)
{
	if (enable_MPU6050 == 0) {
		result->Accel_X_RAW = -999;
		result->Accel_Y_RAW = -999;
		result->Accel_Z_RAW = -999;

		result->Ax = -999;
		result->Ay = -999;
		result->Az = -999;
		return 0;
	}

	uint8_t Rec_Data[6];

	// Read 6 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
	HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	result->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	result->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	result->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	result->Ax = (float)result->Accel_X_RAW/16384.0;
	result->Ay = (float)result->Accel_Y_RAW/16384.0;
	result->Az = (float)result->Accel_Z_RAW/16384.0;

	return 0;
}

int MPU6050_Read_Gyro (MPU6050_Data_t *result)
{
	if (enable_MPU6050 == 0) {
		result->Gyro_X_RAW = -999;
		result->Gyro_Y_RAW = -999;
		result->Gyro_Z_RAW = -999;

		result->Gx = -999;
		result->Gy = -999;
		result->Gz = -999;
		return 0;
	}

	uint8_t Rec_Data[6];

	// Read 6 BYTES of data starting from GYRO_XOUT_H register
	HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	result->Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	result->Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	result->Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	result->Gx = (float)result->Gyro_X_RAW/131.0;
	result->Gy = (float)result->Gyro_Y_RAW/131.0;
	result->Gz = (float)result->Gyro_Z_RAW/131.0;

	return 0;
}

int MPU6050_Read_All (MPU6050_Data_t *result)
{
	if (enable_MPU6050 == 0) {
		result->Accel_X_RAW = -999;
		result->Accel_Y_RAW = -999;
		result->Accel_Z_RAW = -999;
		result->Temp_RAW = - 999;
		result->Gyro_X_RAW = -999;
		result->Gyro_Y_RAW = -999;
		result->Gyro_Z_RAW = -999;

		result->Ax = -999;
		result->Ay = -999;
		result->Az = -999;
		result->Temperature = -999;
		result->Gx = -999;
		result->Gy = -999;
		result->Gz = -999;
		return 0;
	}

	uint8_t Size = 14;
	uint8_t Rec_Data[Size];

	// Read 14 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
	HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, Size, 1000);

	result->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	result->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	result->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
	//Temp
	result->Temp_RAW = (int16_t)(Rec_Data[6] << 8 | Rec_Data [7]);
	//Gyro
	result->Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data [9]);
	result->Gyro_Y_RAW = (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
	result->Gyro_Z_RAW = (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);

	result->Ax = (float)result->Accel_X_RAW/16384.0;
	result->Ay = (float)result->Accel_Y_RAW/16384.0;
	result->Az = (float)result->Accel_Z_RAW/16384.0;
	result->Temperature = (float)result->Temp_RAW/340.0 + 36.53;
	result->Gx = (float)result->Gyro_X_RAW/131.0;
	result->Gy = (float)result->Gyro_Y_RAW/131.0;
	result->Gz = (float)result->Gyro_Z_RAW/131.0;

	return 0;
}

