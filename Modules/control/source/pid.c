/* *
 *
 *
 * */

/* Includes */
#include "pid.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void vInitPIController(PI_Data_Type *pi, float P_gain, float I_gain, float I_limit)
{
	pi->P_gain = fabsf(P_gain); /* Absolute value just in case */
	pi->I_gain = fabsf(I_gain);
	pi->I_limit = fabsf(I_limit);
	pi->I_state = 0.0f;
	pi->Control_signal = 0.0f;
}

void vPI_Update(PI_Data_Type *pi, float reference, float input, float dt)
{
	float p_term, i_term, error;

	/* Create PI error */
	error = reference - input;

    /* Integration with anti-windup */
    pi->I_state = bound(pi->I_limit, -pi->I_limit, pi->I_state + (error * dt));

    /* Include gains */
    p_term = pi->P_gain * error;
    i_term = pi->I_gain * pi->I_state;

    /* Create control signal */
    pi->Control_signal = p_term + i_term;
}
