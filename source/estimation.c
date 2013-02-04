/* *
 *
 *
 * */

/* Includes */
#include "estimation.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
volatile Estimation_State_Struct Estimation_State;
volatile float deg[3] = {0.0f, 0.0f, 0.0f};

/* Private function defines */

void EstimationInit(void)
{
	Estimation_State.q0 = 1.0f;
	Estimation_State.q1 = 0.0f;
	Estimation_State.q2 = 0.0f;
	Estimation_State.q3 = 0.0f;
	Estimation_State.wx = 0.0f;
	Estimation_State.wy = 0.0f;
	Estimation_State.wz = 0.0f;
	Estimation_State.wxb = 0.0f;
	Estimation_State.wyb = 0.0f;
	Estimation_State.wzb = 0.0f;

	xTaskCreate(vTaskRunEstimation,
				"Estimation",
				(1024 * 32) / 4, // 32kB - half of all available
				0,
				configMAX_PRIORITIES - 1,
				0);
}

void vTaskRunEstimation(void *pvParameters)
{
	LEDOn(LED_GREEN);

	while(1)
	{
		xSemaphoreTake(NewMeasurementAvaiable, portMAX_DELAY); /* Wait until the sensors have delivered new data */

		Estimation_State.wx = (float)(Sensor_Data.gyro_x)*DPS2000_TO_RADPS;
		Estimation_State.wy = (float)(Sensor_Data.gyro_y)*DPS2000_TO_RADPS;
		Estimation_State.wz = (float)(Sensor_Data.gyro_z)*DPS2000_TO_RADPS;

		MadgwickAHRSupdate(	Estimation_State.wx,
							Estimation_State.wy,
							Estimation_State.wz,
							-(float)Sensor_Data.acc_x, // The Madgwick algorithm uses a NED-frame
							(float)Sensor_Data.acc_y,
							-(float)Sensor_Data.acc_z, // The Madgwick algorithm uses a NED-frame
							(float)Sensor_Data.mag_x,
							(float)Sensor_Data.mag_y,
							(float)Sensor_Data.mag_z);

		Estimation_State.q0 = q0;
		Estimation_State.q1 = q1;
		Estimation_State.q2 = q2;
		Estimation_State.q3 = q3;

		deg[0] = atan2f(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2))*57.2958f;
		deg[1] = asin(2*(q0*q2 + q1*q3))*57.2958f;
		deg[2] = atan2f(2*(q0*q3 + q2*q1), 1 - 2*(q3*q3 + q2*q2))*57.2958f;

		LEDToggle(LED_GREEN);
	}
}
