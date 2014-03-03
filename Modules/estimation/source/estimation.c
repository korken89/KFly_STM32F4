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

//int16_t raw_data[9000][6];

/* Private function defines */

void EstimationInit(void)
{
	/*xTaskCreate(vTaskRunEstimation,
			"Estimation",
			256, // 16kB
			0,
			configMAX_PRIORITIES - 1,
			0);*/
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
/*
void vTaskRunEstimation(void *pvParameters)
{
	const float throttle = 0.2f;

	Sensor_Raw_Data_Type *data;
	data = ptrGetRawSensorDataPointer();


	uint8_t buff[10];
	uint32_t cnt, i, j;

	vSetRCOutput(0, 0.0f);
	vSetRCOutput(1, 0.0f);
	vSetRCOutput(2, 0.0f);
	vSetRCOutput(3, 0.0f);

	vTaskDelay(10000);

	vSetRCOutput(0, 0.4f);
	vSetRCOutput(1, 0.4f);
	vSetRCOutput(2, 0.4f);
	vSetRCOutput(3, 0.4f);

	vTaskDelay(2000);

	xUSBSendData("Starting data collection - finished in 10 seconds...\r\n", 54);

	for (i = 0; i < 9000; i++)
	{
		xSemaphoreTake(NewMeasurementAvaiable, portMAX_DELAY);

		raw_data[i][0] = data->acc_x;
		raw_data[i][1] = data->acc_y;
		raw_data[i][2] = data->acc_z;

		raw_data[i][3] = data->gyro_x;
		raw_data[i][4] = data->gyro_y;
		raw_data[i][5] = data->gyro_z;
	}

	vSetRCOutput(0, 0.0f);
	vSetRCOutput(1, 0.0f);
	vSetRCOutput(2, 0.0f);
	vSetRCOutput(3, 0.0f);

	xUSBSendData("Finished data collection!\r\n", 27);

	for (i = 0; i < 9000; i++)
	{
		for (j = 0; j < 5; j++)
		{
			cnt = myitoa(raw_data[i][j], buff);
			xUSBSendData((buff + cnt), (11-cnt));
			xUSBSendData(",", 1);
		}

		cnt = myitoa(raw_data[i][j], buff);
		xUSBSendData((buff + cnt), (11-cnt));
		xUSBSendData("\r\n", 2);

		vTaskDelay(10);
	}
	



	while(1)
	{

	}
}
*/