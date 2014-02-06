#ifndef __CONTROL_H
#define __CONTROL_H

/* Standard includes */
#include <math.h>
#include "stm32f4xx.h"

/* System includes */
#include "pwm.h"
#include "pid.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef enum
{
	/* The different flight modes available */
	FLIGHTMODE_RATE = 0,
	FLIGHTMODE_ATTITUDE,
	FLIGHTMODE_VELOCITY,
	FLIGHTMODE_POSITION,
	FLIGHTMODE_POSITION_HOLD
} Flight_Mode_Type;

typedef enum
{
	/* This determines where the system will be looking
	 * while following velocity and position commands */
	TARGET_FIXED_ANGLE = 0,
	TARGET_VELOCITY_VECTOR,
	TARGET_GOAL,
	TARGET_COORDINATE
} Target_Direcion_Type;

typedef struct
{
	/* The general structure containing control reference */
	Flight_Mode_Type mode;
	Target_Direcion_Type target;
	vector3f_t reference;
	float throttle;
} Control_Reference_Type;

typedef struct
{
	/* The controller gains, limits and states of
	 * each controller in the cascade scheme */
	PI_Data_Type position_controller[3];
	PI_Data_Type velocity_controller[3];
	PI_Data_Type attitude_controller[3];
	PI_Data_Type rate_controller[3];
} Control_Data_Type;


typedef struct
{
	float weights[8][4];
} Output_Mixer_Type;

/* Global variable defines */

/* Global function defines */
void vInitControl(void);
void vUpdateOutputs(float, float, float, float);
Control_Data_Type *ptrGetControlData(void);
Output_Mixer_Type *ptrGetOutputMixer(void);

#endif
