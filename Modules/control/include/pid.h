#ifndef __PID_H
#define __PID_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */

/* Scheduler includes */

/* KFly includes */

/* Includes */
#include "trigonometry.h"

/* Defines */

/* Typedefs */
typedef struct
{
	float P_gain;
	float I_gain;
	float I_limit;
	float I_state;
} PI_Data_Type;

/* Global variable defines */

/* Global function defines */
void vInitPIController(PI_Data_Type *, float, float, float);
void vUpdatePISettings(PI_Data_Type *, float, float, float);
float fPIUpdate(PI_Data_Type *, float, float);

#endif
