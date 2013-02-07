#ifndef __CONTROL_H
#define __CONTROL_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */
#include "filters.h"
#include "comlink.h"
#include "crc.h"
#include "estimation.h"
#include "pwm.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef enum
{
	FLIGHTMODE_RATE = 0,
	FLIGHTMODE_ATTITUDE
} Flight_Mode_Type;

typedef struct
{
	Flight_Mode_Type mode;
	float q0;
	float q1;
	float q2;
	float q3;
	float wx;
	float wy;
	float wz;
} Control_Reference_Struct;

/* Global variable defines */

/* Global function defines */
void ControlInit(void);
void vTaskRunControl(void *);

#endif
