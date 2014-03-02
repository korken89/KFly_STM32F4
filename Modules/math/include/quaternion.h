#ifndef __QUATERNION_H
#define __QUATERNION_H

/* Standard includes */
#include "linear_algebra.h"

/* System includes */

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */
#define PI ( 3.14159265359f )

/* Typedefs */
typedef struct
{
	float q0;
	float q1;
	float q2;
	float q3;
} quaternion_t;

typedef struct
{
	float x;
	float y;
	float z;
} vector3f_t;

/* Global variable defines */

/* Global function defines */

/* Includes */
#include "trigonometry.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

/* Global function defines */
void qmult(quaternion_t *, quaternion_t *, quaternion_t *);
void qconj(quaternion_t *);
float qnorm(quaternion_t *);
void qnormalize(quaternion_t *);
void qvqc(quaternion_t *, vector3f_t *, vector3f_t *);
void euler2quat(float, float, float, quaternion_t *);
void q2dcm(float *a, quaternion_t *q);

#endif
