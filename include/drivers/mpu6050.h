#ifndef __MPU6050_H
#define __MPU6050_H

/* Standard includes */
#include "stm32f4xx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */
#define MPU6050_ADDRESS_AD0_LOW     	0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    	0x69 // address pin high (VCC)
#define MPU6050_DEFAULT_ADDRESS     	MPU6050_ADDRESS_AD0_HIGH

/* Global variable defines */

/* Typedefs */

/* Global function defines */
void InitMPU6050(void);



#endif
