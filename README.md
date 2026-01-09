# MPU6050 Data Reader (STM32 HAL + CMSIS-RTOS2)

An STM32 HAL–based **MPU6050** driver and RTOS application that acquires raw IMU data
(accelerometer, gyroscope, temperature) over **I²C**, streams it over **UART**, and performs
all unit conversion and analysis **off-target in MATLAB**.

This project focuses on:
- Correct MPU6050 bring-up and configuration
- Robust I²C communication using STM32 HAL
- RTOS-based task separation (init, acquisition, transport)
- Clean data handoff for external processing

---

## Hardware
- **STM32 Nucleo-L476RG**
- **MPU6050 IMU Sensor** (I²C)
- USB (power + UART via ST-LINK VCP)

### Pin Connections
| MPU6050 | STM32 Nucleo-L476RG |
|-------:|----------------------|
| VCC    | 3.3V                 |
| GND    | GND                  |
| SCL    | PB8 (I2C1_SCL)       |
| SDA    | PB9 (I2C1_SDA)       |
| AD0    | GND *(I²C address = 0x68)* |

---

## Software
- **STM32CubeIDE**
- **STM32 HAL** (I2C1, USART2)
- **CMSIS-RTOS2** (FreeRTOS)
- UART: **115200 baud, 8-N-1**

---

## RTOS Architecture

The application is structured as three RTOS tasks:

### 1) `MPUinit_task` (High Priority)
- Calls `MPU6050_init()`
- Verifies device ID (`WHO_AM_I == 0x68`)
- Configures sensor registers
- On success: releases a semaphore and exits
- On failure: reports error code over UART and retries

### 2) `GetData_task` (Low Priority)
- Waits on initialization semaphore
- Reads raw IMU data using `MPU6050_GetData()`
- Pushes samples into a message queue
- Runs at ~100 Hz (`osDelay(10)`)

### 3) `UART_task` (Normal Priority)
- Blocks on the message queue
- Transmits raw sensor values over UART as ASCII text

This separation prevents blocking I²C reads on UART transmission
and cleanly decouples acquisition from transport.

---

## Driver API

### `HAL_StatusTypeDef MPU6050_init(I2C_HandleTypeDef *hi2c, MPU_ERROR *error)`
Initializes the MPU6050:
- Reads `WHO_AM_I`
- Resets the device
- Selects internal clock
- Configures:
  - DLPF (`CONFIG = 0x03`)
  - Sample rate divider (`SMPLRT_DIV = 0x09`)
  - Gyro range: ±250 °/s
  - Accel range: ±2 g

If initialization fails, a detailed reason is returned via `MPU_ERROR`.

---

### `HAL_StatusTypeDef MPU6050_GetData(I2C_HandleTypeDef *hi2c,
                                      mpu6050_data_t *data,
                                      MPU_ERROR *error)`
- Performs a **single 14-byte burst read** starting at `ACCEL_XOUT_H`
- Populates the following **raw** values:

```text
ax ay az temp gx gy gz
