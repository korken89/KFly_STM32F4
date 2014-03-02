#ifndef __ATTITUDE_EKF_H
#define __ATTITUDE_EKF_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"
#include "sensor_read.h"
#include "linear_algebra.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */
typedef struct
{
	quaternion_t q;
	vector3f_t w;
	vector3f_t wb;
} Attitude_Estimation_States_Type;

typedef struct
{
	float Sp[6][6];		/* Square-root error covariance matrix */
	float Sq[6][6];		/* Square-root process covariance matrix */
	float Sr[3][3];		/* Square-root observation covariance matrix */
} Attitude_Estimation_Settings_Type;

/* Global variable defines */

/* Global function defines */
void AttitudeEstimationInit(void);
void InnovateAttitudeEKF(Attitude_Estimation_States_Type *states, Attitude_Estimation_Settings_Type *settings, Sensor_Data_Type *sensor_data, float dt);

#endif
