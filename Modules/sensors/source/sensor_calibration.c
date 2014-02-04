/* *
 *
 *
 * */

/* Includes */
#include "sensor_calibration.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
static Sensor_Calibration_Type sensor_calibration;

/* Private function defines */


void vInitSensorCalibration(Sensor_Calibration_Type *cal)
{
	/* Set gains and biases */
	cal->accelerometer_bias.x = 0.0f;
	cal->accelerometer_bias.y = 0.0f;
	cal->accelerometer_bias.z = 0.0f;

	cal->accelerometer_gain.x = 1.0f;
	cal->accelerometer_gain.y = 1.0f;
	cal->accelerometer_gain.z = 1.0f;

	cal->magnetometer_bias.x = 0.0f;
	cal->magnetometer_bias.y = 0.0f;
	cal->magnetometer_bias.z = 0.0f;

	cal->magnetometer_gain.x = 1.0f;
	cal->magnetometer_gain.y = 1.0f;
	cal->magnetometer_gain.z = 1.0f;

	cal->gyroscope_gain = GetMPU6050GyroGain();
}

void vLoadSensorCalibration(Sensor_Calibration_Type *cal)
{

}

Sensor_Calibration_Type *ptrGetSensorCalibrationPointer(void)
{
	return &sensor_calibration;
}
