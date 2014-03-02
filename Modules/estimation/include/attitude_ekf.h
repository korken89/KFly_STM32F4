#ifndef __ATTITUDE_EKF_H
#define __ATTITUDE_EKF_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "quaternion.h"
#include "sensor_read.h"
#include <math.h>

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

/* Global variable defines */

/* Global function defines */
void AttitudeEstimationInit(void);
void InnovateAttitudeEKF(Attitude_Estimation_States_Type *states, Sensor_Data_Type *sensor_data);

#endif
