#ifndef __PWM_H
#define __PWM_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"

/* Scheduler includes. */

/* KFly includes */

/* Driver Includes */

/* Includes */

/* Defines */
#define TIMER_RATE		1000000 /* Hz */
#define PEROID_50HZ 	(TIMER_RATE/50 - 1)
#define PEROID_400HZ 	(TIMER_RATE/400 - 1)
#define RC_MIN			0
#define RC_MAX			999

/* Typedefs */
typedef enum
{
	PWM_50Hz = PEROID_50HZ,
	PWM_400Hz = PEROID_400HZ
} PWM_Rate_Type;

typedef enum
{
	OUTPUT_1_TO_4,
	OUTPUT_5_TO_6,
	OUTPUT_7_TO_8
} Output_Group_Type;

typedef enum
{
	RC_CHANNEL1 = 0,
	RC_CHANNEL2,
	RC_CHANNEL3,
	RC_CHANNEL4,
	RC_CHANNEL5,
	RC_CHANNEL6,
	RC_CHANNEL7,
	RC_CHANNEL8
} Output_Channel_Type;

void PWMInit(void);
void vSetRCOutput(Output_Channel_Type, float);
void vSetOutputRate(Output_Group_Type, PWM_Rate_Type);

#endif
