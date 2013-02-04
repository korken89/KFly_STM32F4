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

void IIR_LP_Filter_Init(IIR_LP_Settings *settings, float sampling_freq, float cutoff_freq)
{
	/* *
	 * Simple two pole IIR low-pass filter.
	 *
	 * y[n] = 	b0*x[n] + a1*y[n-1] =
	 * 			hf/(Tf + hf)*x[n] + Tf/(Tf + hf)*y[n-1]
	 * 	where 	hf = sampling time
	 * 			Tf = cut off time (1/(cut off frequency)
	 *
	 * */

	float hf = 1.0f / sampling_freq;
	float Tf = 1.0f / cutoff_freq;

	settings->b0 = hf/(hf+Tf);
	settings->a1 = Tf/(hf+Tf);
	settings->y_old = 0.0f;
}


float IIR_LP_Filter(IIR_LP_Settings *settings, float input)
{
	float output = settings->b0*input + settings->a1*settings->y_old;
	settings->y_old = output;

	return output;
}
