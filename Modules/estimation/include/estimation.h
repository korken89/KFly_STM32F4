#ifndef __ESTIMATION_H
#define __ESTIMATION_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "sensor_read.h"
#include "filters.h"
#include "comlink.h"
#include "crc.h"
#include "quaternion.h"
#include <math.h>

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */
#include "control.h"
#include "MadgwickAHRS.h"

/* Defines */

/* Typedefs */
typedef struct
{
	quaternion_t q;
	vector3f_t w;
	vector3f_t wb;
} Estimation_State_Struct;

/* Global variable defines */
extern volatile Estimation_State_Struct Estimation_State;

/* Global function defines */
void EstimationInit(void);
void vTaskRunEstimation(void *);

#endif
