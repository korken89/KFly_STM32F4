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
				(1024 * 16) / 4, // 16kB
				0,
				configMAX_PRIORITIES - 1,
				0);
}

void vTaskRunEstimation(void *pvParameters)
{
	LEDOn(LED_GREEN);

	IIR_LP_Settings wx_filter, wy_filter, wz_filter;

	IIR_LP_Filter_Init(&wx_filter, 200.0f, 50.0f);
	IIR_LP_Filter_Init(&wy_filter, 200.0f, 50.0f);
	IIR_LP_Filter_Init(&wz_filter, 200.0f, 50.0f);

	while(1)
	{
		/* Wait until the sensors have delivered new data */
		xSemaphoreTake(NewMeasurementAvaiable, portMAX_DELAY);

		Estimation_State.wx = IIR_LP_Filter(&wx_filter, (float)(Sensor_Data.gyro_x)*DPS2000_TO_RADPS);
		Estimation_State.wy = IIR_LP_Filter(&wy_filter, (float)(Sensor_Data.gyro_y)*DPS2000_TO_RADPS);
		Estimation_State.wz = IIR_LP_Filter(&wz_filter, (float)(Sensor_Data.gyro_z)*DPS2000_TO_RADPS);

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

		LEDToggle(LED_GREEN);
	}
}
