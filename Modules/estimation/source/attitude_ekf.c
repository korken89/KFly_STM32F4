/* *
 *
 *
 * */

/* Includes */
#include "attitude_ekf.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
Attitude_Estimation_States_Type Attitude_Estimation_States;

/* Private function defines */

void AttitudeEstimationInit(void)
{

}

void InnovateAttitudeEKF(Attitude_Estimation_States_Type *states, Sensor_Data_Type *sensor_data)
{
	/********************
	 * Prediction Estimate
	 ********************/

	/* 1) Estimate the predicted state: */	 


	/* 2) Estimate the square-root factor of the predicted error covariance matrix: */



	/********************
	 * Measurement Update
	 ********************/

	 /* 1) Subtract the predicted measurement from the true measurement: */


	 /* 2) Estimate the square-root factor of the innovation covariance matrix: */


	 /* 3) Calculate the Kalman gain: */


	 /* 4) Calculate the updated state: */


	 /* 5) Calculate the square-root factor of the corresponding error covariance matrix: */


	 /* 6) Apply the error states to the estimate */
}