/* *
 *
 *
 * */

/* Includes */
#include "quaternion.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void qmult(quaternion_t *a, quaternion_t *b, quaternion_t *r)
{
	/**
	 * q = [a(1)*b(1) - a(2)*b(2) - a(3)*b(3) - a(4)*b(4);
     * 		a(1)*b(2) + a(2)*b(1) + a(3)*b(4) - a(4)*b(3);
     * 		a(1)*b(3) - a(2)*b(4) + a(3)*b(1) + a(4)*b(2);
     * 		a(1)*b(4) + a(2)*b(3) - a(3)*b(2) + a(4)*b(1)]
     * */
	r->q0 = a->q0*b->q0 - a->q1*b->q1 - a->q2*b->q2 - a->q3*b->q3;
	r->q1 = a->q0*b->q1 + a->q1*b->q0 + a->q2*b->q3 - a->q3*b->q2;
	r->q2 = a->q0*b->q2 - a->q1*b->q3 + a->q2*b->q0 + a->q3*b->q1;
	r->q3 = a->q0*b->q3 + a->q1*b->q2 - a->q2*b->q1 + a->q3*b->q0;
}

void qconj(quaternion_t *q)
{
	q->q1 = - q->q1;
	q->q2 = - q->q2;
	q->q3 = - q->q3;
}

float qnorm(quaternion_t *q)
{
	return sqrtf(q->q0*q->q0 + q->q1*q->q1 + q->q2*q->q2 + q->q3*q->q3);
}

void qnormalize(quaternion_t *q)
{
	float invNorm = invSqrt(q->q0*q->q0 + q->q1*q->q1 + q->q2*q->q2 + q->q3*q->q3);

	q->q0 *= invNorm;
	q->q1 *= invNorm;
	q->q2 *= invNorm;
	q->q3 *= invNorm;
}

void qvqc(quaternion_t *q, vector_t *v, vector_t *vr)
{
	/* *
	 *
	 *	v_r = q * v * q'
	 *
	 * */
	quaternion_t qc = {q->q0, - q->q1, - q->q2, - q->q3};
	quaternion_t qv = {0.0f, v->x, v->y, v->z};
	quaternion_t qr1, qr2;

	qmult(q, &qv, &qr1);
	qmult(&qr1, &qc, &qr2);

	vr->x = qr2.q1;
	vr->y = qr2.q2;
	vr->z = qr2.q3;
}

/* *
 * Sine of an angle in radians.
 * Very fast and accurate approximation.
 * */
float sine(float x)
{
	const float B = 4.0f/PI;
	const float C = - 4.0f/(PI*PI);

	float y = B * x + C * x * fabsf(x);

	const float P = 0.225f;

	y = P * (y * fabsf(y) - y) + y;

	return y;
}

float cosine(float x)
{
	return sine(PI/2.0f - x);
}

float tangent(float x)
{
	return (sine(x) / cosine(x));
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
