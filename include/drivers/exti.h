#ifndef __EXTI_H
#define __EXTI_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "led.h"
#include "mpu6050.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Global variable defines */

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

/* Global function defines */
void SensorsInterruptReadInit(void);
void vTaskGetMPU6050Data(void *);
void MPU6050ParseData(void);
void EXTI15_10_IRQHandler(void);

#endif
