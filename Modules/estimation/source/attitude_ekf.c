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
	float w_norm, dtheta, sdtheta, cdtheta;
	quaternion_t dq_int;
	vector3f_t w_hat, theta, mag_B, acc_B, y;

	/* Cast the settings for better looking code, ex: settings->Sp[1][1] is now Sp[1][1] */
	float (*Sp)[6] = settings->Sp;
	float (*Sq)[6] = settings->Sq;
	float (*Sr)[3] = settings->Sr;
	float (*Ss)[3] = settings->Ss;

	/* Calculate w_hat */
	w_hat.x = sensor_data->gyro.x - states->wb.x;
	w_hat.y = sensor_data->gyro.y - states->wb.y;
	w_hat.z = sensor_data->gyro.z - states->wb.z;

	/* Calculate the delta quaternion */
	w_norm = sqrtf(w_hat.x * w_hat.x + w_hat.y * w_hat.y + w_hat.z * w_hat.z);
	dtheta = 0.5f * w_norm * dt;	
	sdtheta = fast_sin(dtheta);
	cdtheta = fast_cos(dtheta);

	dq_int.q0 = cdtheta;
	dq_int.q0 = sdtheta * (w_hat.x / w_norm);
	dq_int.q0 = sdtheta * (w_hat.y / w_norm);
	dq_int.q0 = sdtheta * (w_hat.z / w_norm);

	/* Use the delta quaternion to produce the current estimate of the attitude */
	states->q = qmult(dq_int, states->q);

	/* Calculate dtheta for each axis */
	theta.x = w_hat.x * dt;
	theta.y = w_hat.y * dt;
	theta.z = w_hat.z * dt;

	/* Convert the current quaternion to a DCM */
	q2dcm(&R[0][0], &states->q);


	/****************************
	 *							*
	 *   Prediction Estimate 	*
	 * 							*
	 ****************************/

	/* 1) Estimate the predicted state: */	 

	/* Since the error dynamics predict a zero, no change is made */


	/* 2) Estimate the square-root factor of the predicted error covariance matrix: */

	/* Calculate F_k * Sp_k-1|k-1 part of the QR decomposition */
	Sp[0][0] += Sp[0][1] * theta.z - Sp[0][3] * dt - Sp[0][2] * theta.y;
	Sp[0][1] += Sp[0][2] * theta.x - Sp[0][0] * theta.z - Sp[0][4] * dt;
	Sp[0][2] += Sp[0][0] * theta.y - Sp[0][5] * dt - Sp[0][1] * theta.x;

	Sp[1][0] += Sp[1][1] * theta.z - Sp[1][3] * dt - Sp[1][2] * theta.y;
	Sp[1][1] += Sp[1][2] * theta.x - Sp[1][4] * dt;
	Sp[1][2] += -Sp[1][5] * dt - Sp[1][1] * theta.x;

	Sp[2][0] += -Sp[2][3] * dt - Sp[2][2] * theta.y;
	Sp[2][1] += Sp[2][2] * theta.x - Sp[2][4] * dt;
	Sp[2][2] += -Sp[2][5] * dt;

	Sp[3][0] += -Sp[3][3] * dt;
	Sp[3][1] += -Sp[3][4] * dt;
	Sp[3][2] += -Sp[3][5] * dt;

	Sp[4][1] += -Sp[4][4] * dt;
	Sp[4][2] += -Sp[4][5] * dt;

	Sp[5][2] += -Sp[5][5] * dt;

	/* Perform the QR decomposition: Sp_k|k-1 = QR([F_k * Sp_k-1|k-1, Sq]^T)^T */
	qr_decomp(&Sp[0][0], 12, 6);


	/****************************
	 *							*
	 *    Measurement update 	*
	 * 							*
	 ****************************/

	/* 1) Subtract the predicted measurement from the true measurement: */

	/* Create the measurements */
	acc_B = vecrot_Rtransposed(R, sensor_data->acc);
	mag_B = vecrot_Rtransposed(R, sensor_data->mag);

	/* Since the measurement prediction is based on the states and the 
	   states are zero, then the measurement prediction is zero */
	y.x = - acc_B.y / acc_B.z;
	y.y = - acc_B.x / acc_B.z;
	y.z =   mag_B.y / mag_B.x;

	/* 2) Estimate the square-root factor of the innovation covariance matrix: */
	Ss[0][0] = R[0][0] * Sp[0][0] + R[1][0] * Sp[0][1] + R[2][0] * Sp[0][2]; 
	Ss[0][1] = R[0][1] * Sp[0][0] + R[1][1] * Sp[0][1] + R[2][1] * Sp[0][2]; 
	Ss[0][2] = R[0][2] * Sp[0][0] + R[1][2] * Sp[0][1] + R[2][2] * Sp[0][2]; 

	Ss[1][0] = R[1][0] * Sp[1][1] + R[2][0] * Sp[1][2]; 
	Ss[1][1] = R[1][1] * Sp[1][1] + R[2][1] * Sp[1][2]; 
	Ss[1][2] = R[1][2] * Sp[1][1] + R[2][2] * Sp[1][2]; 

	Ss[2][0] = R[2][0] * Sp[2][2]; 
	Ss[2][1] = R[2][1] * Sp[2][2]; 
	Ss[2][2] = R[2][2] * Sp[2][2];

	/* In the lower half of the Ss matrix I put the observation covariance matrix
	   since the QR decomposition does not distinguish on rows. */
	Ss[3][0] = Sr[0][0]; 
	Ss[3][1] = Sr[0][1];
	Ss[3][2] = Sr[0][2];

	Ss[4][0] = 0.0f; 
	Ss[4][1] = Sr[1][1];
	Ss[4][2] = Sr[1][2];

	Ss[5][0] = 0.0f; 
	Ss[5][1] = 0.0f; 
	Ss[5][2] = Sr[2][2];

	/* Perform the QR decomposition : Ss_k = QR([H_k * Sp_k|k-1, Sr]^T)^T */
	qr_decomp(&Ss[0][0], 6, 3);

	/* 3) Calculate the Kalman gain: */


	/* 4) Calculate the updated state: */


	/* 5) Calculate the square-root factor of the corresponding error covariance matrix: */


	/* 6) Apply the error states to the estimate */


}

