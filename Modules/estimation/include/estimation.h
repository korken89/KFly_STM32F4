#ifndef __ESTIMATION_H
#define __ESTIMATION_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "sensor_read.h"
#include "filters.h"
#include "comlink.h"
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
