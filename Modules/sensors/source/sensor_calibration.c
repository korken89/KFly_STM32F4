/* *
 *
 *
 * */

/* Includes */
#include "sensor_calibration.h"

/* Private Defines */

/* Private Typedefs */

/* Private variable defines */
static Sensor_Calibration_Type sensor_calibration;

/* Global variable defines */

/* Private function defines */


void vInitSensorCalibration(Sensor_Calibration_Type *cal)
{
	/* Set gains and biases */
	cal->accelerometer_bias.x = 17.141f;
	cal->accelerometer_bias.y = -29.604f;
	cal->accelerometer_bias.z = 57.441f;

	cal->accelerometer_gain.x = 2044.294f;
	cal->accelerometer_gain.y = 2046.527f;
	cal->accelerometer_gain.z = 2079.573f;

	cal->magnetometer_bias.x = 97.963f;
	cal->magnetometer_bias.y = 30.558f;
	cal->magnetometer_bias.z = 57.040f;

	cal->magnetometer_gain.x = 683.869f;
	cal->magnetometer_gain.y = 648.546f;
	cal->magnetometer_gain.z = 622.302f;

	/* Set the time stamp */
	cal->timestamp = 0;
}

void vLoadSensorCalibration(Sensor_Calibration_Type *cal)
{

}

Sensor_Calibration_Type *ptrGetSensorCalibration(void)
{
	return &sensor_calibration;
}
