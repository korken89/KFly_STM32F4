#ifndef __SENSOR_READ_H
#define __SENSOR_READ_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "led.h"
#include "mpu6050.h"
#include "hmc5883l.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */
#include "sensor_calibration.h"

/* Defines */

/* Typedefs */
typedef union
{
	uint8_t data[14];
	struct
	{
		int16_t acc_x;
		int16_t acc_y;
		int16_t acc_z;
		int16_t temperature;
		int16_t gyro_x;
		int16_t gyro_y;
		int16_t gyro_z;
	} value;
} MPU6050_Data_Union;

typedef union
{
	uint8_t data[6];
	struct
	{
		int16_t mag_x;
		int16_t mag_z; // For some reason the HMC5883L has stored
		int16_t mag_y; // its axises in the order X, Z, Y
	} value;
} HMC5883L_Data_Union;

typedef struct
{
	float acc_x;
	float acc_y;
	float acc_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float mag_x;
	float mag_y;
	float mag_z;
	int32_t pressure;
} Sensor_Data_Type;

typedef struct
{
	int16_t acc_x;
	int16_t acc_y;
	int16_t acc_z;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
	int16_t mag_x;
	int16_t mag_y;
	int16_t mag_z;
	int32_t pressure;
} Sensor_Raw_Data_Type;

typedef struct
{
	xSemaphoreHandle accelerometer;
	xSemaphoreHandle gyroscope;
	xSemaphoreHandle magnetometer;
	xSemaphoreHandle barometer;
} Sensor_NewMeasurementAvailable_Type;

/* Global variable defines */
extern xSemaphoreHandle NewMeasurementAvaiable;

/* Global function defines */
void SensorsInterruptReadInit(void);
Sensor_Data_Type *ptrGetSensorDataPointer(void);
Sensor_Raw_Data_Type *ptrGetRawSensorDataPointer(void);
void vTaskGetMPU6050Data(void *);
void vTaskGetHMC5883LData(void *);
void EXTI15_10_IRQHandler(void);

#endif
