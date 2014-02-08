/* *
 *
 * General control structure from position to motors:
 *                 __________        __________        ___________        __________        __________        __________
 *                |          |      |          |      |           |      |          |      |          |      |          |
 *            +-> | Position | -+-> | Velocity | ---> | Targeting | -+-> | Attitude | -+-> |   Rate   | -+-> |  Motors  |
 *            |   |__________|  |   |__________|      |___________|  |   |__________|  |   |__________|  |   |__________|
 *            |                 |                                    |                 |                 |
 *             /                 /                                    /                 /                 / <-- Switch
 * Reference -+-----------------+------------------------------------+-----------------+-----------------+
 *
 * Aim:
 * To be able to connect a reference anywhere in the
 * chain in order to choose what type of control to use.
 * This is done by the generic control structure with a
 * setting of the current control mode.
 *
 * The "Targeting" module does conversion from velocity
 * commands to attitude depending on where the system
 * shall be pointing.
 *
 * Every block is an vector PI controller (except Targeting
 * and Motors) with 3 inputs, 3 outputs and 3 references.
 *
 * */

/* Includes */
#include "control.h"

/* Private Defines */

/* Private Typedefs */

/* Private variable defines */
static Control_Reference_Type Control_Reference;
static Control_Data_Type Control_Data;
static Control_Limits_Type Control_Limits;
static Output_Mixer_Type Output_Mixer;

/* Global variable defines */

/* Private function defines */
static vector3f_t vPositionControl(vector3f_t);
static vector3f_t vVelocityControl(vector3f_t);
static vector3f_t vAttitudeControl(vector3f_t);
static void vRateControl(vector3f_t);
static void vUpdateOutputs(float, float, float, float);

void vInitControl(void)
{
	/* Initialize the controllers here */
}

void vUpdateControlAction(Control_Reference_Type *reference)
{
	vector3f_t u;

	switch (reference->mode)
	{
		case FLIGHTMODE_POSITION_HOLD:

			break;

		case FLIGHTMODE_POSITION:
			u = vPositionControl(reference->reference);
			u = vVelocityControl(u);
			u = vAttitudeControl(u);
			vRateControl(u);
			break;

		case FLIGHTMODE_VELOCITY:
			u = vVelocityControl(reference->reference);
			u = vAttitudeControl(u);
			vRateControl(u);
			break;

		case FLIGHTMODE_ATTITUDE:
			u = vAttitudeControl(reference->reference);
			vRateControl(u);
			break;

		case FLIGHTMODE_RATE:
			vRateControl(reference->reference);
			break;

		case FLIGHTMODE_DISARMED:
		default:
			vUpdateOutputs(0.0f, 0.0f, 0.0f, 0.0f);
			break;
	}

}

Control_Data_Type *ptrGetControlData(void)
{
	return &Control_Data;
}

Control_Limits_Type *ptrGetControlLimits(void)
{
	return &Control_Limits;
}

Output_Mixer_Type *ptrGetOutputMixer(void)
{
	return &Output_Mixer;
}


/* *
 *
 * Private functions
 *
 * */

static vector3f_t vPositionControl(vector3f_t reference)
{
	float error_x, error_y, error_z;
	vector3f_t u;

	error_x = reference.x - 0.0f; /* Add where the position can be read */
	error_y = reference.y - 0.0f; /* Add where the position can be read */
	error_z = reference.z - 0.0f; /* Add where the position can be read */

	u.x = fPIUpdate(&Control_Data.position_controller[0], error_x, 0.005f); /* Add where dt is found */
	u.y = fPIUpdate(&Control_Data.position_controller[1], error_y, 0.005f); /* Add where dt is found */
	u.z = fPIUpdate(&Control_Data.position_controller[2], error_z, 0.005f); /* Add where dt is found */

	/* Send control signal to the next step in the cascade */
	return u;

}

static vector3f_t vVelocityControl(vector3f_t reference)
{
	float error_x, error_y, error_z;
	vector3f_t u;


	/* *
	 *
	 * TODO: Add the saturation of velocities.
	 *
	 * */

	error_x = reference.x - 0.0f; /* Add where the velocity can be read */
	error_y = reference.y - 0.0f; /* Add where the velocity can be read */
	error_z = reference.z - 0.0f; /* Add where the velocity can be read */

	u.x = fPIUpdate(&Control_Data.velocity_controller[0], error_x, 0.005f); /* Add where dt is found */
	u.y = fPIUpdate(&Control_Data.velocity_controller[1], error_y, 0.005f); /* Add where dt is found */
	u.z = fPIUpdate(&Control_Data.velocity_controller[2], error_z, 0.005f); /* Add where dt is found */

	/* *
	 *
	 * TODO: Add the conversion block for targeting.
	 *
	 * */

	/* Send control signal to the next step in the cascade */
	return u;
}

static vector3f_t vAttitudeControl(vector3f_t reference)
{
	float error_x, error_y, error_z;
	vector3f_t u;


	/* *
	 *
	 * TODO: Add the saturation of angles.
	 *
	 * */

	/* *
	 *
	 * TODO: Calculate quaternion error.
	 *
	 * */

	u.x = fPIUpdate(&Control_Data.attitude_controller[0], error_x, 0.005f); /* Add where dt is found */
	u.y = fPIUpdate(&Control_Data.attitude_controller[1], error_y, 0.005f); /* Add where dt is found */
	u.z = fPIUpdate(&Control_Data.attitude_controller[2], error_z, 0.005f); /* Add where dt is found */

	/* Send control signal to the next step in the cascade */
	return u;
}

static void vRateControl(vector3f_t reference)
{
	float error_x, error_y, error_z;
	vector3f_t u;


	/* *
	 *
	 * TODO: Add the saturation of rotational rates.
	 *
	 * */

	error_x = reference.x - 0.0f; /* Add where the rotational rate can be read */
	error_y = reference.y - 0.0f; /* Add where the rotational rate can be read */
	error_z = reference.z - 0.0f; /* Add where the rotational rate can be read */

	u.x = fPIUpdate(&Control_Data.rate_controller[0], error_x, 0.005f); /* Add where dt is found */
	u.y = fPIUpdate(&Control_Data.rate_controller[1], error_y, 0.005f); /* Add where dt is found */
	u.z = fPIUpdate(&Control_Data.rate_controller[2], error_z, 0.005f); /* Add where dt is found */

	/* TODO: Send control signal to the engines */
	vUpdateOutputs(Control_Reference.throttle, u.x, u.y, u.z);
}

static void vUpdateOutputs(float u_throttle, float u_pitch, float u_roll, float u_yaw)
{
	float sum;

	for (int i = 0; i < 8; i++)
	{
		sum = u_throttle * Output_Mixer.weights[i][0];
		sum += u_pitch * Output_Mixer.weights[i][1];
		sum += u_roll * Output_Mixer.weights[i][2];
		sum += u_yaw * Output_Mixer.weights[i][3];

		vSetRCOutput(i, sum);
	}
}
