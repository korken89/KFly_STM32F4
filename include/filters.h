#ifndef __FILTERS_H
#define __FILTERS_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include <math.h>

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef struct
{
	float a1;
	float b0;
	float y_old;
} IIR_LP_Settings;

/* Global variable defines */

/* Global function defines */
void IIR_LP_Filter_Init(IIR_LP_Settings *, float, float);
void IIR_LP_Filter(IIR_LP_Settings *);

#endif
