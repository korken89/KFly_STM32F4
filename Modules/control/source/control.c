/* *
 *
 *
 * */

/* Includes */
#include "control.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
volatile Control_Reference_Type Control_Reference;

/* Private function defines */

void ControlInit(void)
{
	Control_Reference.mode = FLIGHTMODE_RATE;

	Control_Reference.q.q0 = 1.0f;
	Control_Reference.q.q1 = 0.0f;
	Control_Reference.q.q2 = 0.0f;
	Control_Reference.q.q3 = 0.0f;

	Control_Reference.w.x = 0.0f;
	Control_Reference.w.y = 0.0f;
	Control_Reference.w.z = 0.0f;

	Control_Reference.throttle = 0.45f;
}

void vTaskRunControl(void *pvParameters)
{
	while(1)
	{
		vTaskSuspend(NULL);
	}
}

void CalcControl(void)
{
	/* Send control commands to outputs */
	//vSetRCOutput(RC_CHANNEL1, ControlSignal2PWMPeriod(u_f));
	//vSetRCOutput(RC_CHANNEL2, ControlSignal2PWMPeriod(u_f));
	//vSetRCOutput(RC_CHANNEL3, ControlSignal2PWMPeriod(u_r));
	//vSetRCOutput(RC_CHANNEL4, ControlSignal2PWMPeriod(u_r));
}

uint32_t ControlSignal2PWMPeriod(float u)
{
	return (uint32_t)(1000.0f * bound(1.0f, 0.0f, u));
}
