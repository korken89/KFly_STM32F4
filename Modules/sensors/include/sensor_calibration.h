#ifndef __SENSOR_CALIBRATION_H
#define __SENSOR_CALIBRATION_H

/* Standard includes */

/* System includes */
#include "mpu6050.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */
#include "quaternion.h"

/* Defines */

/* Typedefs */
typedef struct
{
	vector_t accelerometer_bias;
	vector_t accelerometer_gain;
	vector_t magnetometer_bias;
	vector_t magnetometer_gain;
	float gyroscope_gain;
} Sensor_Calibration_Type;

/* Global variable defines */
extern volatile Sensor_Calibration_Type sensor_calibration;

/* Global function defines */
void vInitSensorCalibration(Sensor_Calibration_Type *);
void vLoadSensorCalibration(Sensor_Calibration_Type *);

#endif
