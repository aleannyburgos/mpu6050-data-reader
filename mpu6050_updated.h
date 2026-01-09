/*
HEADER FILE FOR THE MPU6050. 
THIS WAS MADE USING THE "MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2"
 LINK: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
*/
#ifndef _MPU6050_UPDATED_H
#define _MPU6050_UPDATED_H


//defining the registers that will be used
#define SMPLRT_DIV      0x19
#define SMPLRT_DIV_9    0x09

#define CONFIG          0x1A
#define CONFIG_44HZ     0x03
#define CONFIG_21HZ     0x04

#define GYRO_CONFIG      0x1B
#define GYRO_CONFIG_250  0x00
#define GYRO_CONFIG_500  0x08
#define GYRO_CONFIG_1000 0x10
#define GYRO_CONFIG_2000 0x18

#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG_2G  0x00
#define ACCEL_CONFIG_4G  0x08
#define ACCEL_CONFIG_8G  0x10
#define ACCEL_CONFIG_16G 0x18

#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40

#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42

#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48

#define PWR_MGMT_1        0x6B
#define PWR_MGMT_1_RESET  0x80
#define PWR_MGMT_1_INTCLK 0x00
#define WHO_AM_I          0x75

#define  MPU_I2C_HANDLE_0 (0x68<<1)

typedef struct {
	int16_t ax;
	int16_t ay;
	int16_t az;
	int16_t gx;
	int16_t gy;
	int16_t gz;
	int16_t temp;
} mpu6050_data_t;



typedef enum {
	I2C_ERROR,			//0
	WHOAMI_ERROR,		//1
	RESET_ERROR,		//2
	CLKINIT_ERROR,		//3
	CONFIG_ERROR,		//4
	SMPLRT_ERROR,		//5
	GYROCONFIG_ERROR,	//6
	ACCELCONFIG_ERROR,	//7
	DATAREAD_ERROR		//8
} MPU_ERROR;


//PUBLIC FUNCTIONS
HAL_StatusTypeDef MPU6050_init(I2C_HandleTypeDef *hi2c, MPU_ERROR *error);
HAL_StatusTypeDef MPU6050_GetData(I2C_HandleTypeDef *hi2c, mpu6050_data_t *data, MPU_ERROR *error);

#endif 
