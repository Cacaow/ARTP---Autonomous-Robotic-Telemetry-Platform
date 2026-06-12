#include <MPU6050.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

int enable_MPU6050 = 1;

uint32_t tr;

int finitKalman = 1;

Kalman_t KalmanX = {
    .Q_angle = 0.001f,
    .Q_bias = 0.003f,
    .R_measure = 0.03f};

Kalman_t KalmanY = {
    .Q_angle = 0.001f,
    .Q_bias = 0.003f,
    .R_measure = 0.03f,
};

int MPU6050_init(void)
{
	if (enable_MPU6050 == 0) {
		return 0;
	}

	uint8_t check;
	uint8_t Data;
	HAL_StatusTypeDef status;

	//WHO_AM_I check device ID
	status = HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);  // read WHO_AM_I
	if (check != 0x68 && check != 0x72 )  // if device is not present, return error code
	{
		printf("Error WHO_AM_I fail: %d\n", status);
		return -1;
	}

	//force reset
	/*
	Data = 0x80;
	status = HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, 1000);
	if (status != HAL_OK) {
		printf("Error PWR_MGMT_1_REG fail: %d\n", status);
		return -1;
	}
	HAL_Delay(100);
*/
	// power management register 0X6B we should write all 0's to wake the sensor up
	Data = 0;
	status = HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, 1000);
	if (status != HAL_OK) {
		printf("Error PWR_MGMT_1_REG fail: %d\n", status);
		return -1;
	}
	HAL_Delay(10);

	// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
	Data = 0x07;
	status = HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);
	if (status != HAL_OK) {
		printf("Error SMPLRT_DIV_REG fail: %d\n", status);
		return -1;
	}
/*
	//set DLPF_CFG = 0
	Data = 0x00;
	status = HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, CONFIG_REG, 1, &Data, 1, 1000);
	if (status != HAL_OK) {
		printf("Error CONFIG_REG fail: %d\n", status);
		return -1;
	}
*/

	// Set accelerometer configuration in ACCEL_CONFIG Register
	Data = 0x00;  // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> <strong>±</strong> 2g
	status = HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);
	if (status != HAL_OK) {
		printf("Error ACCEL_CONFIG_REG fail: %d\n", status);
		return -1;
	}


	// Set Gyroscopic configuration in GYRO_CONFIG Register
	Data = 0x00;  // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> <strong>±</strong> 250 ̐/s
	status = HAL_I2C_Mem_Write(&MPU6050_I2C_PORT, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);
	if (status != HAL_OK) {
		printf("Error GYRO_CONFIG_REG fail: %d\n", status);
		return -1;
	}

	finitKalman = 1;

	printf("MPU6050 Initialization process is done!\n");

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
	HAL_StatusTypeDef read_status;
	read_status = HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);
	if (read_status != HAL_OK) {
		printf("Error: Data reading error %d\n", read_status);
	}


	result->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	result->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	result->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	result->Ax = (float)(result->Accel_X_RAW)/16384.0;
	result->Ay = (float)(result->Accel_Y_RAW)/16384.0;
	result->Az = (float)(result->Accel_Z_RAW)/16384.0;

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

	result->Gx = (float)(result->Gyro_X_RAW)/131.0;
	result->Gy = (float)(result->Gyro_Y_RAW)/131.0;
	result->Gz = (float)(result->Gyro_Z_RAW)/131.0;

	return 0;
}


int MPU6050_Calibrate (MPU6050_Data_t *result)
{
	if (enable_MPU6050 == 0) {
		return 0;
	}

	printf("MPU6050 Calibration Start\n");

	result->Ax_RAW_Offset = 0;
	result->Ay_RAW_Offset = 0;
	result->Az_RAW_Offset = 0;
	result->Gx_RAW_Offset = 0;
	result->Gy_RAW_Offset = 0;
	result->Gz_RAW_Offset = 0;

	uint8_t Size = 14;
	uint8_t Rec_Data[Size];
	int Sample_size = 10;

	int32_t Ax_sum = 0;
	int32_t Ay_sum = 0;
	int32_t Az_sum = 0;
	int32_t Gx_sum = 0;
	int32_t Gy_sum = 0;
	int32_t Gz_sum = 0;
	char buffer[1000];

	for (int i = 0; i < Sample_size; i++) {
		// Read 14 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
		HAL_StatusTypeDef read_status;
		read_status = HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, Size, 1000);
		if (read_status != HAL_OK) {
			printf("Error: MPU6050 data reading error %d\n", read_status);
			return -1;
		}

		Ax_sum += (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
		Ay_sum += (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
		Az_sum += (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
		Gx_sum += (int16_t)(Rec_Data[8] << 8 | Rec_Data [9]);
		Gy_sum += (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
		Gz_sum += (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);

		if (i < 10) {

			sprintf(buffer, "MPU6050: Ax_sum: %ld g, Ay_sum: %ld g, Az_sum: %ld g, Gx_sum: %ld Deg/s, Gy_sum: %ld Deg/s, Gz_sum: %ld Deg/s\n",
			    	Ax_sum , Ay_sum, Az_sum , Gx_sum, Gy_sum , Gz_sum);
			printf(buffer);
		}

		HAL_Delay(10);
	}

	result->Ax_RAW_Offset = Ax_sum / Sample_size;
	result->Ay_RAW_Offset = Ay_sum / Sample_size;
	result->Az_RAW_Offset = Az_sum / Sample_size + 16384.0;
	result->Gx_RAW_Offset = Gx_sum / Sample_size;
	result->Gy_RAW_Offset = Gy_sum / Sample_size;
	result->Gz_RAW_Offset = Gz_sum / Sample_size;




    sprintf(buffer, "MPU6050: Ax_Offset: %d g, Ay_Offset: %d g, Az_Offset: %d g, Gx_Offset: %d Deg/s, Gy_Offset: %d Deg/s, Gz_Offset: %d Deg/s\n",
    				result->Ax_RAW_Offset , result->Ay_RAW_Offset, result->Az_RAW_Offset , result->Gx_RAW_Offset, result->Gy_RAW_Offset , result->Gz_RAW_Offset);
    printf(buffer);

	printf("MPU6050 Calibration Finish\n");
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

		result->roll = -999;
		result->pitch = -999;

		result->KalmanAngleY = -999;
		result->KalmanAngleX = -999;

		sprintf(result->state, "Unknown");

		return 0;
	}

	uint8_t Size = 14;
	uint8_t Rec_Data[Size];

	// Read 14 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
	HAL_StatusTypeDef read_status;
	read_status = HAL_I2C_Mem_Read (&MPU6050_I2C_PORT, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, Size, 1000);
	if (read_status != HAL_OK) {
		printf("Error: MPU6050 data reading error %d\n", read_status);
		return -1;
	}

	result->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	result->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	result->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
	//Temp
	result->Temp_RAW = (int16_t)(Rec_Data[6] << 8 | Rec_Data [7]);
	//Gyro
	result->Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data [9]);
	result->Gyro_Y_RAW = (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
	result->Gyro_Z_RAW = (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);

	result->Ax = (float)(result->Accel_X_RAW - result->Ax_RAW_Offset)/16384.0;
	result->Ay = (float)(result->Accel_Y_RAW - result->Ay_RAW_Offset)/16384.0;
	result->Az = (float)(result->Accel_Z_RAW - result->Az_RAW_Offset)/16384.0;
	result->Temperature = (float)(result->Temp_RAW)/340.0 + 36.53;
	result->Gx = (float)(result->Gyro_X_RAW - result->Gx_RAW_Offset)/131.0;
	result->Gy = (float)(result->Gyro_Y_RAW - result->Gy_RAW_Offset)/131.0;
	result->Gz = (float)(result->Gyro_Z_RAW - result->Gz_RAW_Offset)/131.0;

	// Kalman angle solve
	double dt = (double)(HAL_GetTick() - tr) / 1000;
	tr = HAL_GetTick();
	double roll;
	double roll_sqrt = sqrt(
		result->Ax * result->Ax + result->Az * result->Az);
	if (roll_sqrt != 0.0)
	{
		roll = atan(result->Ay / roll_sqrt) * RAD_TO_DEG;
	}
	else
	{
		roll = 0.0;
	}
	double pitch = atan2(-result->Ax, result->Az) * RAD_TO_DEG;
	result->roll = roll;
	result->pitch = pitch;

	//Kalman currently incorrect
	result->KalmanAngleX = roll;
	result->KalmanAngleY = pitch;
	/*
	if (finitKalman)
	{
		KalmanX.angle = roll;
		result->KalmanAngleX = roll;
		KalmanY.angle = pitch;
		result->KalmanAngleY = pitch;
		finitKalman = 0;

	}
	else {
		result->KalmanAngleY = Kalman_getAngle(&KalmanY, pitch, result->Gy, dt);
		result->KalmanAngleX = Kalman_getAngle(&KalmanX, roll, result->Gx, dt);

	}

	if ((pitch < -90 && result->KalmanAngleY > 90) || (pitch > 90 && result->KalmanAngleY < -90))
	{
		KalmanY.angle = pitch;
		result->KalmanAngleY = pitch;
	}
	else
	{

		result->KalmanAngleY = Kalman_getAngle(&KalmanY, pitch, result->Gy, dt);
	}
	if (fabs(result->KalmanAngleY) > 90)
		result->Gx = -result->Gx;
	result->KalmanAngleX = Kalman_getAngle(&KalmanX, roll, result->Gx, dt);
	*/
	if (result->KalmanAngleX > 90) {
		result->KalmanAngleX -= 180;
	}
	else if (result->KalmanAngleX < -90) {
		result->KalmanAngleX += 180;
	}
	if (result->KalmanAngleY > 90) {
		result->KalmanAngleY -= 180;
	}
	else if (result->KalmanAngleY < -90) {
		result->KalmanAngleY += 180;
	}

	int isShaking = 0;
	int isMoving = 0;
	int isTilted = 0;
	int isStable = 0;

	double mov_sqrt = sqrt(
			result->Ax * result->Ax + result->Az * result->Az + result->Ay * result->Ay);

	if (
		abs(result->Gx) > 80 &&
		abs(result->Gy) > 80 &&
		abs(result->Gz) > 80
	) {
		isShaking = 1;
	}

	if (
		mov_sqrt < 0.85 ||
		mov_sqrt > 1.15
	) {
		isMoving = 1;
	}

	if (
		abs(result->KalmanAngleX) > 20 ||
		abs(result->KalmanAngleY) > 20
	) {
		isTilted = 1;
	}

	if (
		abs(result->Gx) < 5 &&
		abs(result->Gy) < 5 &&
		abs(result->Gz) < 5 &&
		abs(result->KalmanAngleX) < 10 &&
		abs(result->KalmanAngleY) < 10
	) {
		isStable = 1;
	}


	if (isShaking) {
		sprintf(result->state, "SHAKING");
	}
	else if (isMoving) {
		sprintf(result->state, "MOVING");
	}
	else if (isTilted) {
		sprintf(result->state, "TILTED");
	}
	else {
		sprintf(result->state, "STABLE");
	}

	return 0;
}

void Kalman_Init(Kalman_t *Kalman) {
    Kalman->angle = 0.0f;
    Kalman->bias = 0.0f;

    Kalman->P[0][0] = 0.0f;
    Kalman->P[0][1] = 0.0f;
    Kalman->P[1][0] = 0.0f;
    Kalman->P[1][1] = 0.0f;

    Kalman->Q_angle = 0.001f;
    Kalman->Q_bias = 0.003f;
    Kalman->R_measure = 0.03f;
}

double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt)
{
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
};
