/* *
 *
 *
 * */

/* Includes */
#include "control.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void ControlInit(void)
{

	xTaskCreate(vTaskRunControl,
				"Estimation",
				(1024 * 1) / 4, // 1kB
				0,
				configMAX_PRIORITIES - 1,
				0);
}

void vTaskRunControl(void *pvParameters)
{
	while(1)
	{

	}
}
