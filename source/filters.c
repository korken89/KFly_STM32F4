/* *
 *
 *
 * */

/* Includes */
#include "filters.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void IIR_LP_Filter_Init(IIR_LP_Settings *settings, float f_c, float f_s)
{
	/* *
	 * Simple single pole IIR low-pass filter.
	 *
	 * y[n] = 	b0*x[n] + a1*y[n-1] =
	 * 			f_c/(f_s+f_c)*x[n] + f_s/(f_s+f_c)*y[n-1]
	 * 	where 	f_s = sampling frequency
	 * 			f_c = cut off frequency
	 *
	 * */
	settings->b0 = f_c/(f_s+f_c);
	settings->a1 = f_s/(f_s+f_c);
	settings->y_old = 0.0f;
}


float IIR_LP_Filter(IIR_LP_Settings *settings, float input)
{
	float output = settings->b0*input + settings->a1*settings->y_old;
	settings->y_old = output;

	return output;
}
