#ifndef __ESTIMATION_H
#define __ESTIMATION_H

/* Standard includes */
#include "stm32f4xx.h"
#include <math.h>

/* System includes */
#include "quaternion.h"
#include "sensor_read.h"
#include "filters.h"
#include "comlink.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */
#include "control.h"

#ifdef __ATTITUDE_EKF_H
	#error "already loaded"
#endif

#include "attitude_ekf.h"

/* Defines */
#define ATTITUDE_ESTIMATION_STATES_SIZE		28

/* Typedefs */

/* Global variable defines */
extern xSemaphoreHandle SemphrEstimationReset;

/* Global function defines */
void EstimationInit(void);
void vTaskRunEstimation(void *);

void ResetEstimation(Attitude_Estimation_States_Type *, Attitude_Estimation_Settings_Type *);
Attitude_Estimation_States_Type *ptrGetAttitudeEstimationStates(void);

#endif
