/* include:
    -sample rate divider 25
    -config 26
    - gyro config 27
    -accel config 28
    -accel measurements 59 - 64
    -temp measurements 65-66
    -gyro measurements 67-72
    -power management 1 107
    -power management 2 108
    -who am i 117
*/
#include <stdint.h>
#include "stm32l4xx_hal.h"        // or stm32xxxx_hal_i2c.h
#include "mpu6050_updated.h"


//initialization function
//check if its able to connect to I2c and is the right address
//enable the pwr management
    //pwr 1
        //10000000 == 0x80 reset
        //00000000 == 0x00 clock
//sample rate divider
    // 00001001 = 0x09  = 1000 hz  / 1 + 9 (sample rate div) = 100 hz
//config
    //00000011- Digital low pass filter smooths out data and reduces noise, 
    //     delay is after reading is read
//gyro config
    //Full scale range, the lower the range the more resolution
    //set to +/- 250
//accel config

HAL_StatusTypeDef MPU6050_init(I2C_HandleTypeDef *hi2c, MPU_ERROR *error)
{
    uint8_t mpu_whoami = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read (
                                    hi2c,
									MPU_I2C_HANDLE_0,
                                    WHO_AM_I,
                                    I2C_MEMADD_SIZE_8BIT,
                                    &mpu_whoami,
                                    1,
                                    1000
                                );

    if (status != HAL_OK){
    	*error = I2C_ERROR;
        return status;
    }

    if (mpu_whoami != 0x68){
    	*error = WHOAMI_ERROR;
        return HAL_ERROR;
    }
        //here we are reseting the device then setting the clock
        uint8_t mpu = PWR_MGMT_1_RESET;
        status = HAL_I2C_Mem_Write (
            hi2c,
			MPU_I2C_HANDLE_0,
            PWR_MGMT_1,    
            I2C_MEMADD_SIZE_8BIT,    
            &mpu,        
            1,          
            1000       
        );
        if (status != HAL_OK) {
        	*error = RESET_ERROR;
            return status;
        }

        HAL_Delay(100); //were going to init after kernal started

        mpu = PWR_MGMT_1_INTCLK;
        status = HAL_I2C_Mem_Write (
            hi2c,
            MPU_I2C_HANDLE_0,
            PWR_MGMT_1,    
            I2C_MEMADD_SIZE_8BIT,    
            &mpu,        
            1,          
            1000       
        );
        if (status != HAL_OK) {
        	*error = CLKINIT_ERROR;
            return status;
        }

        mpu = CONFIG_44HZ;
        status = HAL_I2C_Mem_Write (
            hi2c,
            MPU_I2C_HANDLE_0,
            CONFIG,    
            I2C_MEMADD_SIZE_8BIT,    
            &mpu,        
            1,          
            1000       
        );
        if (status != HAL_OK) {
        	*error = CONFIG_ERROR;
            return status;
        }

        mpu = SMPLRT_DIV_9;
        status = HAL_I2C_Mem_Write (
            hi2c,
            MPU_I2C_HANDLE_0,
            SMPLRT_DIV,    
            I2C_MEMADD_SIZE_8BIT,    
            &mpu,        
            1,          
            1000       
        );
        if (status != HAL_OK) {
        	*error = SMPLRT_ERROR;
            return status;
        }

        mpu = GYRO_CONFIG_250;
        status = HAL_I2C_Mem_Write (
                hi2c,
                MPU_I2C_HANDLE_0,
                GYRO_CONFIG,    
                I2C_MEMADD_SIZE_8BIT,    
                &mpu,        
                1,          
                1000       
            );
        if (status != HAL_OK) {
        	*error = GYROCONFIG_ERROR;
            return status;
        }
        
        mpu = ACCEL_CONFIG_2G;
        status = HAL_I2C_Mem_Write (
            hi2c,
            MPU_I2C_HANDLE_0,
            ACCEL_CONFIG,    
            I2C_MEMADD_SIZE_8BIT,    
            &mpu,        
            1,          
            1000       
        );

        if (status != HAL_OK) {
        	*error = ACCELCONFIG_ERROR;
            return status;
        }
    return HAL_OK;
}

//get data function 
//gets measurements
    //ax,ay,az, gx ,gy ,gz are all 16 bit signed integers
    //the data is stored in a data array that is for 8 bit integers and contains
    //14 elements. This is for all the data.
    // the high data is later shifted and concatenated with the lower data
    // and stored at the address of the variable
HAL_StatusTypeDef MPU6050_GetData(I2C_HandleTypeDef *hi2c, mpu6050_data_t *data,MPU_ERROR *error)
{
    uint8_t out[14];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read (
                hi2c,
                MPU_I2C_HANDLE_0,
                ACCEL_XOUT_H,    
                I2C_MEMADD_SIZE_8BIT,    
                out,
                14,          
                1000       
            );
    if (status != HAL_OK){
    	*error = DATAREAD_ERROR;
        return status;
    }

    data -> ax = (int16_t)((out[0] << 8) | out[1]);
    data -> ay = (int16_t)((out[2] << 8) | out[3]);
    data -> az = (int16_t)((out[4] << 8) | out[5]);
    data -> temp = (int16_t)((out[6] << 8) | out[7]);
    data -> gx = (int16_t)((out[8] << 8) | out[9]);
    data -> gy = (int16_t)((out[10] << 8) | out[11]);
    data -> gz = (int16_t)((out[12] << 8) | out[13]);

    return HAL_OK;
}
