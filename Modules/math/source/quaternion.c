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

}

/*
 * euler2quat - Convert Pitch, Roll and Yaw to quaternion
 * @param[in] roll	    Roll [rad]
 * @param[in] pitch     Pitch [rad]
 * @param[in] yaw		Yaw [rad]
 * @param[out] q    	Quaternion
 */
void euler2quat(float roll, float pitch, float yaw, quaternion_t *q)
{
	/* Prepare the angles for conversion to quaternions */
    pitch *= 0.5f;
    roll  *= 0.5f;
    yaw   *= 0.5f;

    q->q0 = fast_cos(roll) * fast_cos(pitch) * fast_cos(yaw) + fast_sin(roll) * fast_sin(pitch) * fast_sin(yaw);
    q->q1 = fast_sin(roll) * fast_cos(pitch) * fast_cos(yaw) - fast_cos(roll) * fast_sin(pitch) * fast_sin(yaw);
    q->q2 = fast_cos(roll) * fast_sin(pitch) * fast_cos(yaw) + fast_sin(roll) * fast_cos(pitch) * fast_sin(yaw);
    q->q3 = fast_cos(roll) * fast_cos(pitch) * fast_sin(yaw) - fast_sin(roll) * fast_sin(pitch) * fast_cos(yaw);
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

