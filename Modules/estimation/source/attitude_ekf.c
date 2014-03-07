/* *
 *
 *
 * */

/* Includes */
#include "attitude_ekf.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void AttitudeEstimationInit(Attitude_Estimation_States_Type *states,
							Attitude_Estimation_Settings_Type *settings,
							quaternion_t *start_attitude,
							vector3f_t *start_bias,
							float dt)
{
	float s_q, s_b, s_a, s_t, s_p;

	/* Initialize states */
	states->q.q0 = start_attitude->q0;
	states->q.q1 = start_attitude->q1;
	states->q.q2 = start_attitude->q2;
	states->q.q3 = start_attitude->q3;

	states->wb.x = start_bias->x;
	states->wb.y = start_bias->y;
	states->wb.z = start_bias->z;


	/*
	 * Initialize the gain matrices
	 */

	/* Cast the settings for better looking code, ex: settings->Sp[1][1] is now Sp[1][1] */
	float (*Sp)[6] = settings->Sp;
	float (*T1)[6] = settings->T1;
	float (*Sq)[6] = settings->Sq;
	float (*Ss)[3] = settings->Ss;
	float (*T2)[3] = settings->T2;
	float (*Sr)[3] = settings->Sr;
	float (*T3)[6] = settings->T3;
	float (*K)[3] = settings->K;

	/* Model settings */
	s_q = 0.01f;
	s_b = 0.001f*dt;

	/* Measurement settings */
	s_a = 100.0f;
	s_t = 1000.0f;

	/* Error setting */
	s_p = 10.0;

	/* Zero matrices*/
	create_zero(&Sp[0][0], 6, 6);
	create_zero(&Sq[0][0], 6, 6);
	create_zero(&Sr[0][0], 3, 3);
	create_zero(&Ss[0][0], 3, 3);
	create_zero(&T1[0][0], 6, 6);
	create_zero(&T2[0][0], 3, 3);
	create_zero(&T3[0][0], 3, 6);
	create_zero(&K[0][0], 6, 3);

	/* Model covariance */
	Sq[0][0] = s_q;
	Sq[1][1] = s_q;
	Sq[2][2] = s_q;

	Sq[3][3] = s_b;
	Sq[4][4] = s_b;
	Sq[5][5] = s_b;

	Sq[0][3] = -0.5f * s_b;
	Sq[1][4] = -0.5f * s_b;
	Sq[2][5] = -0.5f * s_b;

	Sq[3][0] = -0.5f * s_b;
	Sq[4][1] = -0.5f * s_b;
	Sq[5][2] = -0.5f * s_b;

	/* Create the model square-root factor */
	chol_decomp_upper(&Sq[0][0], 6);

	/* Mesurement covariance */
	Sr[0][0] = s_a;
	Sr[1][1] = s_a;
	Sr[2][2] = s_t;

	/* Create the measurement square-root factor */
	chol_decomp_upper(&Sr[0][0], 3);


	/* Set the starting error covariance matrix */
	Sp[0][0] = s_p;
	Sp[1][1] = s_p;
	Sp[2][2] = s_p;
	Sp[3][3] = s_p;
	Sp[4][4] = s_p;
	Sp[5][5] = s_p;

	/* Create the error covariance square-root factor */
	chol_decomp_upper(&Sp[0][0], 6);
	
	
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param states [description]
 * @param settings [description]
 * @param gyro [description]
 * @param beta [description]
 * @param u [description]
 * @param dt [description]
 */
void InnovateAttitudeEKF(	Attitude_Estimation_States_Type *states,
							Attitude_Estimation_Settings_Type *settings, 
							float gyro[3],
							float acc[3],
							float mag[3],
							float beta,
							float u,
							float dt)
{
	uint32_t i, j;
	float R[3][3];
	float w_norm, dtheta, sdtheta, cdtheta, t1, t2, t3, x_hat[6];
	quaternion_t dq_int;
	vector3f_t w_hat, theta, mag_v, acc_v, mag_B, acc_B, y;

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
	w_hat.x = gyro[0] - states->wb.x;
	w_hat.y = gyro[1] - states->wb.y;
	w_hat.z = gyro[2] - states->wb.z;

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

	/* Calculate F_k * Sp_k-1|k-1 part of the QR decomposition, with (I + F_k) * Sp_k-1|k-1 */
	t1 = Sp[0][0];
	t2 = Sp[0][1];
	t3 = Sp[0][2];

	Sp[0][0] += t2 * theta.z - Sp[0][3] * dt - t3 * theta.y;
	Sp[0][1] += t3 * theta.x - t1 * theta.z - Sp[0][4] * dt;
	Sp[0][2] += t1 * theta.y - Sp[0][5] * dt - t2 * theta.x;

	t2 = Sp[1][1];
	t3 = Sp[1][2];

	Sp[1][0] += t2 * theta.z - Sp[1][3] * dt - t3 * theta.y;
	Sp[1][1] += t3 * theta.x - Sp[1][4] * dt;
	Sp[1][2] += -Sp[1][5] * dt - t2 * theta.x;

	t3 = Sp[2][2];

	Sp[2][0] += -Sp[2][3] * dt - t3 * theta.y;
	Sp[2][1] += t3 * theta.x - Sp[2][4] * dt;
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
	acc_v.x = acc[0];
	acc_v.y = acc[1];
	acc_v.z = acc[2];

	mag_v.x = mag[0];
	mag_v.y = mag[1];
	mag_v.z = mag[2];

	acc_B = vector_rotation_transposed(R, acc_v);
	mag_B = vector_rotation_transposed(R, mag_v);

	/* Since the measurement prediction is based on the states and the 
	   states are zero, then the measurement prediction is zero and the
	   error is the measurement directly. */
	y.x = - acc_B.y / acc_B.z;
	y.y =   acc_B.x / acc_B.z;
	y.z =   mag_B.y / mag_B.x;


	/*
	 * 2) Estimate the square-root factor of the innovation covariance matrix:
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
	T2[0][0] = Sr[0][0]; 
	T2[0][1] = Sr[0][1];
	T2[0][2] = Sr[0][2];

	T2[1][0] = 0.0f; 
	T2[1][1] = Sr[1][1];
	T2[1][2] = Sr[1][2];

	T2[2][0] = 0.0f; 
	T2[2][1] = 0.0f; 
	T2[2][2] = Sr[2][2];

	/* Perform the QR decomposition : Ss_k = QR([H_k * Sp_k|k-1, Sr]^T) */
	qr_decomp_tria(&Ss[0][0], 3);

	/* Invert Ss, since we only need the inverted version for future calculations */
	u_inv(&Ss[0][0], 3);

	/* Create T3 = Ss^-1 * H * Sp */
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

	/* K = Sp * T3^T * Ss^-1 */
	t1 = Sp[0][0] * T3[0][0];
	t2 = Sp[0][0] * T3[1][0];
	t3 = Sp[0][0] * T3[2][0];

	K[0][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[0][1] = 				  Ss[1][1] * t2 + Ss[1][2] * t3;
	K[0][2] = 					  			  Ss[2][2] * t3;

	t1 = Sp[0][1] * T3[0][0] + Sp[1][1] * T3[0][1];
	t2 = Sp[0][1] * T3[1][0] + Sp[1][1] * T3[1][1];
	t3 = Sp[0][1] * T3[2][0] + Sp[1][1] * T3[2][1];

	K[1][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[1][1] = 				  Ss[1][1] * t2 + Ss[1][2] * t3;
	K[1][2] = 					  			  Ss[2][2] * t3;

	t1 = Sp[0][2] * T3[0][0] + Sp[1][2] * T3[0][1] + Sp[2][2] * T3[0][2];
	t2 = Sp[0][2] * T3[1][0] + Sp[1][2] * T3[1][1] + Sp[2][2] * T3[1][2];
	t3 = Sp[0][2] * T3[2][0] + Sp[1][2] * T3[2][1] + Sp[2][2] * T3[2][2];

	K[2][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[2][1] = 				  Ss[1][1] * t2 + Ss[1][2] * t3;
	K[2][2] = 					  			  Ss[2][2] * t3;

	t1 = Sp[0][3] * T3[0][0] + Sp[1][3] * T3[0][1] + Sp[2][3] * T3[0][2];
	t2 = Sp[0][3] * T3[1][0] + Sp[1][3] * T3[1][1] + Sp[2][3] * T3[1][2];
	t3 = Sp[0][3] * T3[2][0] + Sp[1][3] * T3[2][1] + Sp[2][3] * T3[2][2];

	K[3][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[3][1] = 				  Ss[1][1] * t2 + Ss[1][2] * t3;
	K[3][2] = 					  			  Ss[2][2] * t3;

	t1 = Sp[0][4] * T3[0][0] + Sp[1][4] * T3[0][1] + Sp[2][4] * T3[0][2];
	t2 = Sp[0][4] * T3[1][0] + Sp[1][4] * T3[1][1] + Sp[2][4] * T3[1][2];
	t3 = Sp[0][4] * T3[2][0] + Sp[1][4] * T3[2][1] + Sp[2][4] * T3[2][2];

	K[4][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[4][1] = 				  Ss[1][1] * t2 + Ss[1][2] * t3; 
	K[4][2] = 					  			  Ss[2][2] * t3;

	t1 = Sp[0][5] * T3[0][0] + Sp[1][5] * T3[0][1] + Sp[2][5] * T3[0][2];
	t2 = Sp[0][5] * T3[1][0] + Sp[1][5] * T3[1][1] + Sp[2][5] * T3[1][2];
	t3 = Sp[0][5] * T3[2][0] + Sp[1][5] * T3[2][1] + Sp[2][5] * T3[2][2];

	K[5][0] = Ss[0][0] * t1 + Ss[0][1] * t2 + Ss[0][2] * t3;
	K[5][1] = 				  Ss[1][1] * t2 + Ss[1][2] * t3;
	K[5][2] = 					  			  Ss[2][2] * t3;


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

 	/* Perform the Cholesky downdate with each column of T3 */
 	chol_downdate(&T1[0][0], &T3[0][0], 6);
 	chol_downdate(&T1[0][0], &T3[1][0], 6);
 	chol_downdate(&T1[0][0], &T3[2][0], 6);

 	/* Create the updated error covariance matrix Sp = T1 * Sp 
 	   (the chol_downdate creates an upper triangular matrix, no transpose needed)  */
 	uu_mul(&T1[0][0], &Sp[0][0], 6);

	/*
	 * 6) Apply the error states to the estimate
	 */
	/* TODO: Change to GRP conversion */
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

	/*
	 *		End of filter!
	 */	
}

