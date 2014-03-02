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

void qvqc(quaternion_t *q, vector3f_t *v, vector3f_t *vr)
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

/*
 * euler2quat - Convert Pitch, Roll and Yaw to quaternion
 * @param[in] r	    Roll [rad]
 * @param[in] p     Pitch [rad]
 * @param[in] y     Yaw [rad]
 * @param[out] q    Quaternion
 */
void euler2quat(float r, float p, float y, quaternion_t *q)
{

}


void q2dcm(float *a, quaternion_t *q)
{
	/*
	  R = [q(1)^2+q(2)^2-q(3)^2-q(4)^2,       2*(q(2)*q(3)-q(1)*q(4)),       2*(q(2)*q(4)+q(1)*q(3));
               2*(q(2)*q(3)+q(1)*q(4)),   q(1)^2-q(2)^2+q(3)^2-q(4)^2,       2*(q(3)*q(4)-q(1)*q(2));
               2*(q(2)*q(4)-q(1)*q(3)),       2*(q(3)*q(4)+q(1)*q(2)),   q(1)^2-q(2)^2-q(3)^2+q(4)^2];
    */
	float q0, q1, q2, q3;
	const int n = 3;

	q0 = q->q0;
	q1 = q->q1;
	q2 = q->q2;
	q3 = q->q3;

	A(0, 0) = q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3;
	A(0, 1) = 2.0f * (q1 * q2 - q0 * q3);
	A(0, 2) = 2.0f * (q1 * q3 + q0 * q2);

	A(1, 0) = 2.0f * (q1 * q2 + q0 * q3);
	A(1, 1) = q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3;
	A(1, 2) = 2.0f * (q2 * q3 - q0 * q1);

	A(2, 0) = 2.0f * (q1 * q3 - q0 * q2);
	A(2, 1) = 2.0f * (q2 * q3 + q0 * q1);
	A(2, 2) = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;
}

