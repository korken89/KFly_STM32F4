#ifndef __ATTITUDE_EKF_H
#define __ATTITUDE_EKF_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"
#include "sensor_read.h"
#include "linear_algebra.h"
#include "trigonometry.h"
#include "debug_print.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */
#define DT 		(0.005f)

/* Process covariances */
#define SQ_Q	(0.01f)
#define SQ_B 	(0.001f * DT)

/* Observation covariances */
#define SR_A 	(100.0f)
#define SR_T 	(1000.0f)

/* Starting error covariance */
#define S_P 	(10.0f)

/* Calculate the square-root factors (from Matlab) */
#define SQ_1 	(sqrtf(SQ_Q))
#define SQ_2 	(0.5f * sqrtf(SQ_B * (4.0f * SQ_Q - SQ_B) / SQ_Q))
#define SQ_3 	(-SQ_B / (2.0f * sqrtf(SQ_Q)))

#define SR_1 	(sqrtf(SR_A))
#define SR_2 	(sqrtf(SR_T))


/* Typedefs */
typedef struct
{
	quaternion_t q;
	vector3f_t wb;
} Attitude_Estimation_States_Type;

typedef struct
{
	float Sp[6][6];		/* Square-root error covariance matrix 			*/
	float T1[6][6];		/* Temporary matrix 							*/
	float Ss[3][3];		/* Square-root innovation covariance matrix 	*/
	float T2[3][3];		/* Temporary matrix 							*/
	float T3[3][6];		/* Temporary matrix 							*/
	float  K[6][3];		/* Kalman gain matrix							*/
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
							float u_sum,
							float dt);

#endif
