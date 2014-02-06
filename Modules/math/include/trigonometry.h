#ifndef __TRIGONOMETRY_H
#define __TRIGONOMETRY_H

/* Standard includes */
#include <math.h>

/* System includes */

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */
#define PI ( 3.14159265359f )

/* Typedefs */

/* Global variable defines */

/* Global function defines */

/* Includes */

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Inline functions */
static inline float bound(float max, float min, float x)
{
	if (x > max)
		return max;
	else if (x < min)
		return min;
	else
		return x;
}

/* Private function defines */

/* Global function defines */
float fast_sin(float);
float fast_cos(float);
float invSqrt(float);
float fmodf(float, float);
float myfloor(float);

#endif
