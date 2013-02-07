/* *
 *
 *
 * */

/* Includes */
#include "control.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
volatile Control_Reference_Struct ControlReference;

/* Private function defines */

void ControlInit(void)
{
	ControlReference.mode = FLIGHTMODE_RATE;

	ControlReference.q.q0 = 1.0f;
	ControlReference.q.q1 = 0.0f;
	ControlReference.q.q2 = 0.0f;
	ControlReference.q.q3 = 0.0f;

	ControlReference.w.x = 0.0f;
	ControlReference.w.y = 0.0f;
	ControlReference.w.z = 0.0f;
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
	/* *
	 *
	 * Control scheme:
	 *
	 * - Rate mode: P-controller
	 *	u_r = P_w *(w
	 *
	 * - Attitude mode: P-controller
	 *  u_a
	 *
	 * */
	float P_q = 100.0f;
	float P_w = 100.0f;



	uint32_t u_r = (uint32_t)(P_w * (Estimation_State.w.x - ControlReference.w.x));


}
