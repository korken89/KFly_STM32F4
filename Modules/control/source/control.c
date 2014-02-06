/* *
 *
 *
 * */

/* Includes */
#include "control.h"

/* Private Defines */

/* Private Typedefs */

/* Private variable defines */
static Control_Data_Type Control_Data;
static Output_Mixer_Type Output_Mixer;

/* Global variable defines */

/* Private function defines */


void vInitControl(void)
{
	/* Initialize the controllers here */
}

void vUpdateOutputs(float u_throttle, float u_pitch, float u_roll, float u_yaw)
{
	float sum;

	for (int i = 0; i < 8; i++)
	{
		sum = u_throttle * Output_Mixer.weights[i][0];
		sum += u_pitch * Output_Mixer.weights[i][1];
		sum += u_roll * Output_Mixer.weights[i][2];
		sum += u_yaw * Output_Mixer.weights[i][3];

		vSetRCOutput(i, sum);
	}
}

Control_Data_Type *ptrGetControlData(void)
{
	return &Control_Data;
}

Output_Mixer_Type *ptrGetOutputMixer(void)
{
	return &Output_Mixer;
}
