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
#include "quaternion.h"

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
	quaternion_t q;
	vector_t w;
} Control_Reference_Struct;

/* Global variable defines */

/* Global function defines */
void ControlInit(void);
void vTaskRunControl(void *);

#endif
