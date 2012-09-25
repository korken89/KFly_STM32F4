#ifndef __PWM_H
#define __PWM_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */

/* Scheduler includes. */

/* KFly includes */

/* Driver Includes */

/* Includes */

/* Defines */
#define TIMER_RATE		1000000 /* Hz */
#define PEROID_50HZ 	(20000 - 1)
#define PEROID_400HZ 	(2500 - 1)

/* Typedefs */
typedef enum
{
	PWM_50Hz,
	PWM_400Hz
} PWM_Rate_Type;

typedef enum
{
	OUTPUT_1_TO_4,
	OUTPUT_5_TO_6,
	OUTPUT_7_TO_8
} Output_Number_Type;

void InitPWM(void);

#endif
