/* *
 *
 *
 * */

/* Includes */
#include "estimation.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */
Estimation_State_Struct Estimation_State;

/* Private function defines */

void EstimationInit(void)
{
	Estimation_State.q.q0 = 1.0f;
	Estimation_State.q.q1 = 0.0f;
	Estimation_State.q.q2 = 0.0f;
	Estimation_State.q.q3 = 0.0f;
	Estimation_State.wb.x = 0.0f;
	Estimation_State.wb.y = 0.0f;
	Estimation_State.wb.z = 0.0f;

	xTaskCreate(vTaskRunEstimation,
			"Estimation",
			(1024 * 16) / 4, // 16kB
			0,
			configMAX_PRIORITIES - 1,
			0);
}

void vTaskRunEstimation(void *pvParameters)
{
	while(1)
	{
		/* Wait until the sensors have delivered new data */
		// xSemaphoreTake(NewMeasurementAvaiable, portMAX_DELAY);
	}
}
