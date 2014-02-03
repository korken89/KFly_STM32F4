/* *
 *
 *
 * */

/* Includes */
#include "trig.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */


/*
 * fast_sin - Sine approximation
 * @param[in] x	    Angle [rad]
 * @param[out] y    Sine
 */
float fast_sin(float x)
{
	if (x < -PI)
	    x += 2.0f * PI;
	else if (x > PI)
	    x -= 2.0f * PI;

	const float B = 4.0f/PI;
	const float C = -4.0f/(PI*PI);
	const float P = 0.225f;

	float y = B * x + C * x * fabsf(x);
	y = P * (y * fabsf(y) - y) + y;

	return y;
}

float fast_cos(float x)
{
	x += PI*0.5f;

	if (x < -PI)
	    x += 2.0f * PI;
	else if (x > PI)
	    x -= 2.0f * PI;

	const float B = 4.0f/PI;
	const float C = -4.0f/(PI*PI);
	const float P = 0.225f;

	float y = B * x + C * x * fabsf(x);
	y = P * (y * fabsf(y) - y) + y;

	return y;
}

float bound(float max, float min, float x)
{
	if (x > max)
		return max;
	else if (x < min)
		return min;
	else
		return x;
}

float invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

float fmodf(float x, float m)
{
	return x - myfloor(x / m) * m;
}

float myfloor(float x)
{
	if (x > 0.0f)
		return (int)x;
	else
		return (int)(x - 0.9999999999999999f);
}
