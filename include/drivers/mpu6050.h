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
#include "i2c.h"

/* Defines */
#define MPU6050_ADDRESS_AD0_LOW    	0x68 // Address pin low (GND)
#define MPU6050_ADDRESS_AD0_HIGH   	0x69 // Address pin high (VCC)
#define MPU6050_ADDRESS     		MPU6050_ADDRESS_AD0_HIGH

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void InitMPU6050(void);
ErrorStatus GetMUP6050ID(uint8_t *);


#endif
