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

Control_Data_Type *ptrGetControlData(void)
{
	return &Control_Data;
}

Output_Mixer_Type *ptrGetOutputMixer(void)
{
	return &Output_Mixer;
}
