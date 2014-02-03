#ifndef __PID_H
#define __PID_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef enum
{
	CONTROL_P = 0,
	CONTROL_PD,
	CONTROL_PI,
	CONTROL_PID
} Controller_Type;

typedef struct
{
	Controller_Type type;
	float Integral;

} PID_Data_Type;


/* Global variable defines */

/* Global function defines */

#endif
