/* *
 *
 *
 * */

/* Includes */
#include "control.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
volatile Control_Reference_Struct Control_Reference;

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

	const float P_q = 5.0f;
	const float P_w = 0.1f;

	quaternion_t q_err;
	quaternion_t qc_m = {Estimation_State.q.q0,
						-Estimation_State.q.q1,
						-Estimation_State.q.q2,
						-Estimation_State.q.q3};

	/* Calculate quaternion error */
	qmult(&Control_Reference.q, &qc_m, &q_err);

	Control_Reference.w.x = P_q * q_err.q1;

	float u_f = Control_Reference.throttle;
	float u_r = u_f;
	u_f += (-P_w * (Estimation_State.w.x - Control_Reference.w.x));
	u_r -= (-P_w * (Estimation_State.w.x - Control_Reference.w.x));

	vSetRCOutput(RC_CHANNEL1, ControlSignal2PWMPeriod(u_f));
	vSetRCOutput(RC_CHANNEL2, ControlSignal2PWMPeriod(u_f));
	vSetRCOutput(RC_CHANNEL3, ControlSignal2PWMPeriod(u_r));
	vSetRCOutput(RC_CHANNEL4, ControlSignal2PWMPeriod(u_r));
}

uint32_t ControlSignal2PWMPeriod(float u)
{
	return (uint32_t)(1000.0f * bound(1.0f, 0.0f, u));
}
