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
Attitude_Estimation_Settings_Type Attitude_Estimation_Settings;

/* Private function defines */

void AttitudeEstimationInit(void)
{
	/* Initialize states */
	Attitude_Estimation_States.q.q0 = 1.0f;
	Attitude_Estimation_States.q.q1 = 0.0f;
	Attitude_Estimation_States.q.q2 = 0.0f;
	Attitude_Estimation_States.q.q3 = 0.0f;

	Attitude_Estimation_States.w.x = 0.0f;
	Attitude_Estimation_States.w.y = 0.0f;
	Attitude_Estimation_States.w.z = 0.0f;

	Attitude_Estimation_States.wb.x = 0.0f;
	Attitude_Estimation_States.wb.y = 0.0f;
	Attitude_Estimation_States.wb.z = 0.0f;
}

void InnovateAttitudeEKF(	Attitude_Estimation_States_Type *states,
							Attitude_Estimation_Settings_Type *settings, 
							Sensor_Data_Type *sensor_data,
							float dt)
{
	float R[3][3];
	float theta_x, theta_y, theta_z;

	/* Cast the settings for better looking code, ex: settings->Sp[1][1] is now Sp[1][1] */
	float (*Sp)[6] = settings->Sp;
	float (*Sq)[6] = settings->Sq;
	float (*Sr)[3] = settings->Sr;

	/* Convert the current quaternion to a DCM */
	q2dcm(&R[0][0], &states->q);

	/* Calculate dtheta for each axis based on the qyros, gyro bias and time step */
	theta_x = (sensor_data->gyro_x - states->wb.x) * dt;
	theta_y = (sensor_data->gyro_y - states->wb.y) * dt;
	theta_z = (sensor_data->gyro_z - states->wb.z) * dt;

	/****************************
	 *							*
	 *   Prediction Estimate 	*
	 * 							*
	 ****************************/

	/* 1) Estimate the predicted state: */	 

	/* Since the error dynamics predict a zero, no change is made */


	/* 2) Estimate the square-root factor of the predicted error covariance matrix: */

	/* Calculate F_k * Sp_k-1|k-1 part of the QR decomposition */
	Sp[0][0] += Sp[0][1] * theta_z - Sp[0][3] * dt - Sp[0][2] * theta_y;
	Sp[0][1] += Sp[0][2] * theta_x - Sp[0][0] * theta_z - Sp[0][4] * dt;
	Sp[0][2] += Sp[0][0] * theta_y - Sp[0][5] * dt - Sp[0][1] * theta_x;

	Sp[1][0] += Sp[1][1] * theta_z - Sp[1][3] * dt - Sp[1][2] * theta_y;
	Sp[1][1] += Sp[1][2] * theta_x - Sp[1][4] * dt;
	Sp[1][2] += - Sp[1][5] * dt - Sp[1][1] * theta_x;

	Sp[2][0] += - Sp[2][3] * dt - Sp[2][2] * theta_y;
	Sp[2][1] += Sp[2][2] * theta_x - Sp[2][4] * dt;
	Sp[2][2] += -Sp[2][5] * dt;

	Sp[3][0] += -Sp[3][3] * dt;
	Sp[3][1] += -Sp[3][4] * dt;
	Sp[3][2] += -Sp[3][5] * dt;

	Sp[4][1] += -Sp[4][4] * dt;
	Sp[4][2] += -Sp[4][5] * dt;

	Sp[5][2] += -Sp[5][5] * dt;

	/* Perform the QR decomposition: Sp_k|k-1 = QR([F_k * Sp_k-1|k-1, Sq]^T) */
	qr_decomp(&Sp[0][0], 12, 6);


	/****************************
	 *							*
	 *    Measurement update 	*
	 * 							*
	 ****************************/

	 /* 1) Subtract the predicted measurement from the true measurement: */

	 /* Create the measurement */


	 /* 2) Estimate the square-root factor of the innovation covariance matrix: */


	 /* 3) Calculate the Kalman gain: */


	 /* 4) Calculate the updated state: */


	 /* 5) Calculate the square-root factor of the corresponding error covariance matrix: */


	 /* 6) Apply the error states to the estimate */


}

