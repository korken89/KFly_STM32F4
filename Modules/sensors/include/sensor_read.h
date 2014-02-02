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
} Raw_Sensor_Data_Struct;

/* Global variable defines */
extern volatile Raw_Sensor_Data_Struct Sensor_Data;
extern xSemaphoreHandle NewMeasurementAvaiable;

/* Global function defines */
void SensorsInterruptReadInit(void);
void vTaskGetMPU6050Data(void *);
void vTaskGetHMC5883LData(void *);
void EXTI15_10_IRQHandler(void);

#endif
