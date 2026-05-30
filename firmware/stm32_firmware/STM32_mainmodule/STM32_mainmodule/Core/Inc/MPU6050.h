#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f4xx_hal.h"

#include <stddef.h>
#include <stdint.h>
#include <_ansi.h>


#define MPU6050_I2C_PORT        hi2c1
#define MPU6050_ADDR 				(0x68 << 1) 		/*!I2C Address of MPU6050, which is 0xD0*/
extern I2C_HandleTypeDef MPU6050_I2C_PORT;

/*
 *	 ===============================================================================
 *                     	   ##### MPU6050 Register address Macros #####
 *	 ===============================================================================
 */

#define WHO_AM_I_REG			(0x75)	//
#define PWR_MGMT_1_REG 			(0x6B)
#define SMPLRT_DIV_REG 			(0x19)
#define CONFIG_REG 				(0x1A)
#define GYRO_CONFIG_REG 		(0x1B)
#define ACCEL_CONFIG_REG 		(0x1C)
#define ACCEL_XOUT_H_REG		(0x3B)
#define GYRO_XOUT_H_REG			(0x43)
#define TEMP_OUT_H_REG			(0x41)

#define RAD_TO_DEG 57.295779513082320876798154814105

//Functions


/**
  * MPU6050 raw data structure definition
  */

typedef struct {
    uint16_t Accel_X_RAW;
	uint16_t Accel_Y_RAW;
	uint16_t Accel_Z_RAW;
	float Ax;
	float Ay;
	float Az;

	uint16_t Temp_RAW;
	float Temperature;

	uint16_t Gyro_X_RAW;
	uint16_t Gyro_Y_RAW;
	uint16_t Gyro_Z_RAW;
	float Gx;
	float Gy;
	float Gz;

	float roll;
	float pitch;

	double KalmanAngleX;
	double KalmanAngleY;
}MPU6050_Data_t;

// Kalman structure
typedef struct
{
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;

int MPU6050_init(void);
int MPU6050_close(void);
int MPU6050_Read_Accel (MPU6050_Data_t *result);
int MPU6050_Read_Gyro (MPU6050_Data_t *result);
int MPU6050_Read_All (MPU6050_Data_t *result);
double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt);

#endif /* INC_MPU6050_H_ */
