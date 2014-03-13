/* *
 *
 *
 * */

/* Includes */
#include "estimation.h"
#include "debug_print.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */	

/* Private function defines */
xSemaphoreHandle SemphrEstimationReset = NULL;
Attitude_Estimation_States_Type *attitude_ekf_states = NULL;

void EstimationInit(void)
{
	vSemaphoreCreateBinary(SemphrEstimationReset);
	xSemaphoreGive(SemphrEstimationReset);

	xTaskCreate(vTaskRunEstimation,
				"Attitude Estimation",
				2048,
				0,
				configMAX_PRIORITIES - 2,
				0);
}

void vTaskRunEstimation(void *pvParameters)
{
	Attitude_Estimation_States_Type states;
	Attitude_Estimation_Settings_Type settings;
	Sensor_Data_Type *sensor_data;

	attitude_ekf_states = &states;
	sensor_data = ptrGetSensorDataPointer();
	
	vTaskDelay(1000);

	while(1)
	{
		if (xSemaphoreTake(SemphrEstimationReset, 0) == pdTRUE)
        	ResetEstimation(&states, &settings);

		xSemaphoreTake(NewMeasurementAvaiable, portMAX_DELAY);

		InnovateAttitudeEKF(&states,
							&settings, 
							(float *)&sensor_data->gyro,
							(float *)&sensor_data->acc,
							(float *)&sensor_data->mag,
							0.0f,
							0.0f,
							0.005f);
	}
}

void ResetEstimation(Attitude_Estimation_States_Type *states, Attitude_Estimation_Settings_Type *settings)
{
	uint32_t i;

	quaternion_t q_init;
	vector3f_t wb_init = {0.0f, 0.0f, 0.0f};
	vector3f_t acc_init = {0.0f, 0.0f, 0.0f};
	vector3f_t mag_init = {0.0f, 0.0f, 0.0f};

	Sensor_Data_Type *sensor_data;

	sensor_data = ptrGetSensorDataPointer();


	/* Take 100 measurements to create a starting guess for the filter */
	for (i = 0; i < 100; i++)
	{
		xSemaphoreTake(NewMeasurementAvaiable, portMAX_DELAY);

		acc_init = vector_add(acc_init, sensor_data->acc);
		mag_init = vector_add(mag_init, sensor_data->mag);
	}

	/* Scale all the measurements to get the mean value */
	acc_init = vector_scale(acc_init, 1.0f / ((float) i));
	mag_init = vector_scale(mag_init, 1.0f / ((float) i));

	/* Generate the starting guess quaternion */
	GenerateStartingGuess(&acc_init, &mag_init, &q_init);

	/* Initialize the estimation */
	AttitudeEstimationInit(states, settings, &q_init, &wb_init, 0.005f);
}

Attitude_Estimation_States_Type *ptrGetAttitudeEstimationStates(void)
{
	return attitude_ekf_states;
}