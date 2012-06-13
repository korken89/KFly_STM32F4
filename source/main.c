#include "main.h"


USB_OTG_CORE_HANDLE USB_OTG_dev;

void main(void)
{
	/* *
	 *
	 * Initialization of pherials and I/O-ports
	 *
	 * */


	/* *
	 *
	 * LED init.
	 * Initializes and sets up the port for the LEDs as outputs.
	 * Red LED on Port B, Pin 0
	 * Green LED on Port B, Pin 1
	 *
	 * */
	LEDInit();
	vUSBQueueInit();
	/* *
	 *
	 * 	USB init.
	 * 	Running USB Full speed - 12Mbps as Virtual COM Port via the CDC interface
	 * 	Shows as ttyACM0 in Ubuntu and COMxx in Windows.
	 * 	Linux version does not need a driver but Windows version uses ST serial driver.
	 *
	 * */
	USBD_Init(	&USB_OTG_dev,
				USB_OTG_FS_CORE_ID,
				&USR_desc,
				&USBD_CDC_cb,
				&USR_cb);

	xTaskCreate(vTaskCode,
				"NAME1",
				256,
				0,
				tskIDLE_PRIORITY+1,
		        0);

	vTaskStartScheduler();

	/* We only get here if there was insuficient memory to start the Scheduler */

	while (1);
}

void vTaskCode(void *pvParameters)
{
	char text;

	while(1)
	{
		xQueueReceive(xUSBQueueHandle, &text, portMAX_DELAY);

		if (text == 'a' || text == 'A')
		{
			xUSBSendData("LED On\n\r", 9);
			LEDOn(RED);
		}

		//if there is an 's' in buffer
		else if (text == 's' || text == 'S')
		{
			xUSBSendData("LED Off\n\r", 10);
			LEDOff(RED);
		}

		//vTaskDelay(250 / portTICK_RATE_MS);
		//LEDToggle(RED);
	}
}
