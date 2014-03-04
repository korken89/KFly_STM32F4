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
	uint32_t i, j;
	float R[3][3];
	float w_norm, dtheta, sdtheta, cdtheta, t1, t2, t3, x_hat[6];
	quaternion_t dq_int;
	vector3f_t w_hat, theta, mag_B, acc_B, y;

	/* Cast the settings for better looking code, ex: settings->Sp[1][1] is now Sp[1][1] */
	float (*Sp)[6] = settings->Sp;
	float (*T1)[6] = settings->T1;
	float (*Sq)[6] = settings->Sq;
	float (*Ss)[3] = settings->Ss;
	float (*T2)[3] = settings->T2;
	float (*Sr)[3] = settings->Sr;
	float (*T3)[6] = settings->T3;
	float (*K)[3] = settings->K;

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
	dq_int.q1 = sdtheta * (w_hat.x / w_norm);
	dq_int.q2 = sdtheta * (w_hat.y / w_norm);
	dq_int.q3 = sdtheta * (w_hat.z / w_norm);

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

	/*
	 * 1) Estimate the predicted state:
	 */	 

	/* Since the error dynamics predict a zero, no change is made */


	/* 
	 * 2) Estimate the square-root factor of the predicted error covariance matrix:
	 */

	/* Calculate F_k * Sp_k-1|k-1 part of the QR decomposition */

/*
[ Sp01*theta_z - Sp03*dt - Sp02*theta_y, Sp02*theta_x - Sp00*theta_z - Sp04*dt, Sp00*theta_y - Sp05*dt - Sp01*theta_x, 0, 0, 0]
[ Sp11*theta_z - Sp13*dt - Sp12*theta_y,                Sp12*theta_x - Sp14*dt,              - Sp15*dt - Sp11*theta_x, 0, 0, 0]
[              - Sp23*dt - Sp22*theta_y,                Sp22*theta_x - Sp24*dt,                              -Sp25*dt, 0, 0, 0]
[                              -Sp33*dt,                              -Sp34*dt,                              -Sp35*dt, 0, 0, 0]
[                                     0,                              -Sp44*dt,                              -Sp45*dt, 0, 0, 0]
[                                     0,                                     0,                              -Sp55*dt, 0, 0, 0]
*/
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

	/* Copy the Sq values to the temporary matrix for use in the decomposition */
	T1[0][0] = Sq[0][0];
	T1[0][1] = Sq[0][1];
	T1[0][2] = Sq[0][2];
	T1[0][3] = Sq[0][3];
	T1[0][4] = Sq[0][4];
	T1[0][5] = Sq[0][5];

	T1[1][1] = Sq[1][1];
	T1[1][2] = Sq[1][2];
	T1[1][3] = Sq[1][3];
	T1[1][4] = Sq[1][4];
	T1[1][5] = Sq[1][5];

	T1[2][2] = Sq[2][2];
	T1[2][3] = Sq[2][3];
	T1[2][4] = Sq[2][4];
	T1[2][5] = Sq[2][5];

	T1[3][3] = Sq[3][3];
	T1[3][4] = Sq[3][4];
	T1[3][5] = Sq[3][5];

	T1[4][4] = Sq[4][4];
	T1[4][5] = Sq[4][5];

	T1[5][5] = Sq[5][5];

	/* Perform the QR decomposition: Sp_k|k-1 = QR([F_k * Sp_k-1|k-1, Sq]^T) */
	qr_decomp_tria(&Sp[0][0], 6);


	/****************************
	 *							*
	 *    Measurement update 	*
	 * 							*
	 ****************************/

	/*
	 * 1) Subtract the predicted measurement from the true measurement:
	 */

	/* Create the measurements */
	acc_B = vector_rotation_transposed(R, sensor_data->acc);
	mag_B = vector_rotation_transposed(R, sensor_data->mag);

	/* Since the measurement prediction is based on the states and the 
	   states are zero, then the measurement prediction is zero and the
	   error is the measurement directly. */
	y.x = - acc_B.y / acc_B.z;
	y.y = - acc_B.x / acc_B.z;
	y.z =   mag_B.y / mag_B.x;

	/*
	 * 2) Estimate the square-root factor of the innovation covariance matrix:
	 */
/*
[ R00*Sp00 + R10*Sp01 + R20*Sp02, R01*Sp00 + R11*Sp01 + R21*Sp02, R02*Sp00 + R12*Sp01 + R22*Sp02]
[            R10*Sp11 + R20*Sp12,            R11*Sp11 + R21*Sp12,            R12*Sp11 + R22*Sp12]
[                       R20*Sp22,                       R21*Sp22,                       R22*Sp22]
[                              0,                              0,                              0]
[                              0,                              0,                              0]
[                              0,                              0,                              0]
*/
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
	T2[3][0] = Sr[0][0]; 
	T2[3][1] = Sr[0][1];
	T2[3][2] = Sr[0][2];

	T2[4][0] = 0.0f; 
	T2[4][1] = Sr[1][1];
	T2[4][2] = Sr[1][2];

	T2[5][0] = 0.0f; 
	T2[5][1] = 0.0f; 
	T2[5][2] = Sr[2][2];

	/* Perform the QR decomposition : Ss_k = QR([H_k * Sp_k|k-1, Sr]^T) */
	qr_decomp_tria(&Ss[0][0], 3);

	/* Invert Ss, since we only need the inverted version for future calculations */
	u_inv(&Ss[0][0], 3);

	/* Create T3 = Ss^-1 * H * Sp */
/*
[                                                                         R00*Sp00*Ss00 + R10*Sp01*Ss00 + R20*Sp02*Ss00,                                                 R10*Sp11*Ss00 + R20*Sp12*Ss00,                         R20*Sp22*Ss00, 0, 0, 0]
[                                  Sp00*(R00*Ss01 + R01*Ss11) + Sp01*(R10*Ss01 + R11*Ss11) + Sp02*(R20*Ss01 + R21*Ss11),                       Sp11*(R10*Ss01 + R11*Ss11) + Sp12*(R20*Ss01 + R21*Ss11),            Sp22*(R20*Ss01 + R21*Ss11), 0, 0, 0]
[ Sp00*(R00*Ss02 + R01*Ss12 + R02*Ss22) + Sp01*(R10*Ss02 + R11*Ss12 + R12*Ss22) + Sp02*(R20*Ss02 + R21*Ss12 + R22*Ss22), Sp11*(R10*Ss02 + R11*Ss12 + R12*Ss22) + Sp12*(R20*Ss02 + R21*Ss12 + R22*Ss22), Sp22*(R20*Ss02 + R21*Ss12 + R22*Ss22), 0, 0, 0]
*/
	t1 = R[0][0] * Ss[0][0];
	t2 = R[1][0] * Ss[0][0];
	t3 = R[2][0] * Ss[0][0];

	T3[0][0] = Sp[0][0] * t1 + Sp[0][1] * t2 + Sp[0][2] * t3;
	T3[0][1] = Sp[1][1] * t2 + Sp[1][2] * t3;
	T3[0][2] = Sp[2][2] * t3;

	t1 = R[0][0] * Ss[0][1] + R[0][1] * Ss[1][1];
	t2 = R[1][0] * Ss[0][1] + R[1][1] * Ss[1][1];
	t3 = R[2][0] * Ss[0][1] + R[2][1] * Ss[1][1];

	T3[1][0] = Sp[0][0] * t1 + Sp[0][1] * t2 + Sp[0][2] * t3;
	T3[1][1] = Sp[1][1] * t2 + Sp[1][2] * t3;
	T3[1][2] = Sp[2][2] * t3;

	t1 = R[0][0] * Ss[0][2] + R[0][1] * Ss[1][2] + R[0][2] * Ss[2][2];
	t2 = R[1][0] * Ss[0][2] + R[1][1] * Ss[1][2] + R[1][2] * Ss[2][2];
	t3 = R[2][0] * Ss[0][2] + R[2][1] * Ss[1][2] + R[2][2] * Ss[2][2];

	T3[2][0] = Sp[0][0] * t1 + Sp[0][1] * t2 + Sp[0][2] * t3;
	T3[2][1] = Sp[1][1] * t2 + Sp[1][2] * t3;
	T3[2][2] = Sp[2][2] * t3;

	/*
	 * 3) Calculate the Kalman gain 
	 */

	/* K = Sp * T2^T * Ss^-1 */
/*
[                                                                               Sp00*Ss00*T200 + Sp00*Ss01*T210 + Sp00*Ss02*T220,                                                     Sp00*Ss11*T210 + Sp00*Ss12*T220,                           Sp00*Ss22*T220]
[                                     Ss00*(Sp01*T200 + Sp11*T201) + Ss01*(Sp01*T210 + Sp11*T211) + Ss02*(Sp01*T220 + Sp11*T221),                         Ss11*(Sp01*T210 + Sp11*T211) + Ss12*(Sp01*T220 + Sp11*T221),             Ss22*(Sp01*T220 + Sp11*T221)]
[ Ss00*(Sp02*T200 + Sp12*T201 + Sp22*T202) + Ss01*(Sp02*T210 + Sp12*T211 + Sp22*T212) + Ss02*(Sp02*T220 + Sp12*T221 + Sp22*T222), Ss11*(Sp02*T210 + Sp12*T211 + Sp22*T212) + Ss12*(Sp02*T220 + Sp12*T221 + Sp22*T222), Ss22*(Sp02*T220 + Sp12*T221 + Sp22*T222)]
[ Ss00*(Sp03*T200 + Sp13*T201 + Sp23*T202) + Ss01*(Sp03*T210 + Sp13*T211 + Sp23*T212) + Ss02*(Sp03*T220 + Sp13*T221 + Sp23*T222), Ss11*(Sp03*T210 + Sp13*T211 + Sp23*T212) + Ss12*(Sp03*T220 + Sp13*T221 + Sp23*T222), Ss22*(Sp03*T220 + Sp13*T221 + Sp23*T222)]
[ Ss00*(Sp04*T200 + Sp14*T201 + Sp24*T202) + Ss01*(Sp04*T210 + Sp14*T211 + Sp24*T212) + Ss02*(Sp04*T220 + Sp14*T221 + Sp24*T222), Ss11*(Sp04*T210 + Sp14*T211 + Sp24*T212) + Ss12*(Sp04*T220 + Sp14*T221 + Sp24*T222), Ss22*(Sp04*T220 + Sp14*T221 + Sp24*T222)]
[ Ss00*(Sp05*T200 + Sp15*T201 + Sp25*T202) + Ss01*(Sp05*T210 + Sp15*T211 + Sp25*T212) + Ss02*(Sp05*T220 + Sp15*T221 + Sp25*T222), Ss11*(Sp05*T210 + Sp15*T211 + Sp25*T212) + Ss12*(Sp05*T220 + Sp15*T221 + Sp25*T222), Ss22*(Sp05*T220 + Sp15*T221 + Sp25*T222)]
*/
	t1 = Sp[0][0] * T2[0][0];
	t2 = Sp[0][0] * T2[1][0];
	t3 = Sp[0][0] * T2[2][0];

	K[0][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[0][1] = Ss[1][1] * t2 + Ss[1][2] * t3;
	K[0][2] = Ss[2][2] * t3;

	t1 = Sp[0][1] * T2[0][0] + Sp[1][1] * T2[0][1];
	t2 = Sp[0][1] * T2[1][0] + Sp[1][1] * T2[1][1];
	t3 = Sp[0][1] * T2[2][0] + Sp[1][1] * T2[2][1];

	K[1][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[1][1] = Ss[1][1] * t2 + Ss[1][2] * t3;
	K[1][2] = Ss[2][2] * t3;

	t1 = Sp[0][2] * T2[0][0] + Sp[1][2] * T2[0][1] + Sp[2][2] * T2[0][2];
	t2 = Sp[0][2] * T2[1][0] + Sp[1][2] * T2[1][1] + Sp[2][2] * T2[1][2];
	t3 = Sp[0][2] * T2[2][0] + Sp[1][2] * T2[2][1] + Sp[2][2] * T2[2][2];

	K[2][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[2][1] = Ss[1][1] * t2 + Ss[1][2] * t3;
	K[2][2] = Ss[2][2] * t3;

	t1 = Sp[0][3] * T2[0][0] + Sp[1][3] * T2[0][1] + Sp[2][3] * T2[0][2];
	t2 = Sp[0][3] * T2[1][0] + Sp[1][3] * T2[1][1] + Sp[2][3] * T2[1][2];
	t3 = Sp[0][3] * T2[2][0] + Sp[1][3] * T2[2][1] + Sp[2][3] * T2[2][2];

	K[3][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[3][1] = Ss[1][1] * t2 + Ss[1][2] * t3;
	K[3][2] = Ss[2][2] * t3;

	t1 = Sp[0][4] * T2[0][0] + Sp[1][4] * T2[0][1] + Sp[2][4] * T2[0][2];
	t2 = Sp[0][4] * T2[1][0] + Sp[1][4] * T2[1][1] + Sp[2][4] * T2[1][2];
	t3 = Sp[0][4] * T2[2][0] + Sp[1][4] * T2[2][1] + Sp[2][4] * T2[2][2];

	K[4][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[4][1] = Ss[1][1] * t2 + Ss[1][2] * t3;
	K[4][2] = Ss[2][2] * t3;

	t1 = Sp[0][5] * T2[0][0] + Sp[1][5] * T2[0][1] + Sp[2][5] * T2[0][2];
	t2 = Sp[0][5] * T2[1][0] + Sp[1][5] * T2[1][1] + Sp[2][5] * T2[1][2];
	t3 = Sp[0][5] * T2[2][0] + Sp[1][5] * T2[2][1] + Sp[2][5] * T2[2][2];

	K[5][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[5][1] = Ss[1][1] * t2 + Ss[1][2] * t3;
	K[5][2] = Ss[2][2] * t3;


	/* 
	 * 4) Calculate the updated state: 
	 */
/*
 K00*yx + K01*yy + K02*yz
 K10*yx + K11*yy + K12*yz
 K20*yx + K21*yy + K22*yz
 K30*yx + K31*yy + K32*yz
 K40*yx + K41*yy + K42*yz
 K50*yx + K51*yy + K52*yz
*/
 	x_hat[0] = K[0][0] * y.x + K[0][1] * y.y + K[0][2] * y.z;
 	x_hat[1] = K[1][0] * y.x + K[1][1] * y.y + K[1][2] * y.z;
 	x_hat[2] = K[2][0] * y.x + K[2][1] * y.y + K[2][2] * y.z;
 	x_hat[3] = K[3][0] * y.x + K[3][1] * y.y + K[3][2] * y.z;
 	x_hat[4] = K[4][0] * y.x + K[4][1] * y.y + K[4][2] * y.z;
 	x_hat[5] = K[5][0] * y.x + K[5][1] * y.y + K[5][2] * y.z;

	/*
	 * 5) Calculate the square-root factor of the corresponding error covariance matrix:
	 */

	/* Create an 6x6 identity matrix */
 	create_identity(&T1[0][0], 6);

 	/* Perform the Cholesky downdate */
 	chol_downdate(&T1[0][0], &T3[0][0], 6);
 	chol_downdate(&T1[0][0], &T3[1][0], 6);
 	chol_downdate(&T1[0][0], &T3[2][0], 6);

 	/* Create the updated error covariance matrix
 	   (the chol_downdate creates an upper triangular matrix, no transpose needed)  */

 	/* Form  Sp = T1 * Sp */
 	uu_mul(&T1[0][0], &Sp[0][0], 6);

	/*
	 * 6) Apply the error states to the estimate
	 */
/*
q_upd = [ 1; % (eq 238)
           dq];
q_upd = q_upd / norm(q_upd);
q_hat = qmult(q_upd, q_hat); % Quaternion update (eq 240)  
wb_hat = wb_hat +  dwb; % Bias update
*/
	dq_int.q0 = 1;
	dq_int.q1 = 0.5f * x_hat[0];
	dq_int.q2 = 0.5f * x_hat[1];
	dq_int.q3 = 0.5f * x_hat[2];

	/* Make sure the quaternion has length one */
	qnormalize(&dq_int);

	/* Use the delta quaternion to produce the current estimate of the attitude */
	states->q = qmult(dq_int, states->q);

	/* Update the estimation of the bias */
	states->wb.x += x_hat[3];
	states->wb.y += x_hat[4];
	states->wb.z += x_hat[5];

	/* Update the angular rate */
	states->w.x = sensor_data->gyro.x - states->wb.x;
	states->w.y = sensor_data->gyro.y - states->wb.y;
	states->w.z = sensor_data->gyro.z - states->wb.z;

	/*
	 *		End of filter!
	 */
}

