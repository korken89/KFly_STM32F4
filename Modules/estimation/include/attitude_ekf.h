#ifndef __ATTITUDE_EKF_H
#define __ATTITUDE_EKF_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"
#include "sensor_read.h"
#include "linear_algebra.h"
#include "trigonometry.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef struct
{
	quaternion_t q;
	vector3f_t wb;
} Attitude_Estimation_States_Type;

typedef struct
{
	float Sp[6][6];		/* Square-root error covariance matrix */
	float T1[6][6];		/* Temporary matrix for the QR calculations */
	float Sq[6][6];		/* Square-root process covariance matrix */
	float Ss[3][3];		/* Square-root innovation covariance matrix */
	float T2[3][3];
	float Sr[3][3];		/* Square-root observation covariance matrix */
	float T3[3][6];
	float  K[6][3];		/* Kalman gain */
} Attitude_Estimation_Settings_Type;

/* Global variable defines */

/* Global function defines */
void AttitudeEstimationInit(Attitude_Estimation_States_Type *states,
							Attitude_Estimation_Settings_Type *settings,
							quaternion_t *start_attitude,
							vector3f_t *start_bias,
							float dt);

void InnovateAttitudeEKF(	Attitude_Estimation_States_Type *states,
							Attitude_Estimation_Settings_Type *settings, 
							float gyro[3],
							float acc[3],
							float mag[3],
							float beta,
							float u,
							float dt);

#endif
