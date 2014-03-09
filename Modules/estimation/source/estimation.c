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



void EstimationInit(void)
{
	xTaskCreate(vTaskRunEstimation,
				"Estimation",
				2048, // 16kB
				0,
				configMAX_PRIORITIES - 2,
				0);
}

volatile float gyro[3] = {0.01f, 0.01f, 0.01f};
volatile float acc[3] = {0.0f, 0.0f, 9.81f};
volatile float mag[3] = {5.0f, 0.0f, 1.0f};

void vTaskRunEstimation(void *pvParameters)
{
	uint32_t i = 0;

	Attitude_Estimation_States_Type states;
	Attitude_Estimation_Settings_Type settings;

	quaternion_t q_init;

	q_init.q0 = 1.0f;
	q_init.q1 = 0.0f;
	q_init.q2 = 0.0f;
	q_init.q3 = 0.0f;

	vector3f_t wb_init;

	wb_init.x = 0.0f;
	wb_init.y = 0.0f;
	wb_init.z = 0.0f;

	AttitudeEstimationInit(&states, &settings, &q_init, &wb_init, 0.005f);

	while(1)
	{
		vTaskDelay(10000);

		InnovateAttitudeEKF(&states,
							&settings, 
							(float *)gyro,
							(float *)acc,
							(float *)mag,
							0.0f,
							0.0f,
							0.005f);

	}
}


uint32_t myitoa(int16_t num, uint8_t *buf)
{
	int32_t i;

	if (num == 0)
	{
		buf[10] = '0';
		return 10;
	}

	if (num < 0)
	{
		num = -num;

		for(i = 10; (num && i); i--)
		{
			buf[i] = "0123456789"[num % 10];
			num /= 10;
		}

		buf[i--] = '-';
	}
	else
	{
		for(i = 10; (num && i); i--)
		{
			buf[i] = "0123456789"[num % 10];
			num /= 10;
		}
	}
	
	return (i+1);
}

