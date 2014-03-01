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
#define SENSOR_CALIBERATION_SIZE  		(4*3*4 + 4)

/* Typedefs */
typedef struct
{
	vector3f_t accelerometer_bias;
	vector3f_t accelerometer_gain;
	vector3f_t magnetometer_bias;
	vector3f_t magnetometer_gain;
	uint32_t timestamp;
} Sensor_Calibration_Type;

/* Global variable defines */

/* Global function defines */
void vInitSensorCalibration(Sensor_Calibration_Type *);
void vLoadSensorCalibration(Sensor_Calibration_Type *);
Sensor_Calibration_Type *ptrGetSensorCalibration(void);

#endif
