#ifndef __ESTIMATION_H
#define __ESTIMATION_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "sensor_read.h"
#include "filters.h"
#include <math.h>

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */
#include "MadgwickAHRS.h"

/* Defines */

/* Typedefs */
typedef struct
{
	float q0;
	float q1;
	float q2;
	float q3;
	float wx;
	float wy;
	float wz;
	float wxb;
	float wyb;
	float wzb;
} Estimation_State_Struct;

/* Global variable defines */
extern volatile Estimation_State_Struct Estimation_State;

/* Global function defines */
void EstimationInit(void);
void vTaskRunEstimation(void *);

#endif
