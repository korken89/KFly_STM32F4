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
	FLIGHTMODE_ATTITUDE,
	FLIGHTMODE_VELOCITY,
	FLIGHTMODE_POSITION
} Flight_Mode_Type;

typedef struct
{
	Flight_Mode_Type mode;
	quaternion_t q;
	vector_t w;
	float throttle;
} Control_Reference_Type;

typedef struct
{
	float mix_weights[8][4];
} Output_Mixer_Type;

/* Global variable defines */

/* Global function defines */
void ControlInit(void);
void vTaskRunControl(void *);
void CalcControl(void);
Control_Reference_Type *ptrGetControlReference(void);
Output_Mixer_Type *ptrGetOutputMixer(void);
uint32_t ControlSignal2PWMPeriod(float);

#endif
