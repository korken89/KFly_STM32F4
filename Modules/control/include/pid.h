#ifndef __PID_H
#define __PID_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */

/* Scheduler includes */

/* KFly includes */

/* Includes */
#include "trig.h"

/* Defines */

/* Typedefs */
typedef struct
{
	float P_gain;
	float I_gain;
	float I_state;
	float I_limit;
	float Control_signal;
} PI_Data_Type;


/* Global variable defines */

/* Global function defines */
void vInitPIController(PI_Data_Type *, float, float, float);
void pi_update(PI_Data_Type *, float, float, const float);

#endif
