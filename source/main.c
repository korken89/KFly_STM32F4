#include "main.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;

char* itoa(int val, int base)
{
	static char buf[31] = {0};
	if (val == 0)
	{
		buf[0] = '0';
		return buf;
	}

	int i;
	for(i = 30; (val && i); --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];
}

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

	/* *
	 *
	 * USB recieve queue init
	 *
	 * */
	vUSBQueueInit();

	/* *
	 *
	 * FastCounter init.
	 * Initializes and sets up the TIM2 as a 1MHz counter for timing.
	 *
	 * */
	InitFastCounter();

	/* *
	 *
	 * 	USB init.
	 * 	Running USB Full speed - 12Mbps as Virtual COM Port via the CDC interface
	 * 	Shows as ttyACM0 in Ubuntu and COMxx in Windows.
	 * 	Linux version does not need a driver but Windows version uses STM serial driver.
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

	xTaskCreate(vTaskPrintTimer,
				"TIMER",
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

		if (text == 'a')
		{
			xUSBSendData("LED On\n\r", 9);
			LEDOn(RED);
		}

		else if (text == 's')
		{
			xUSBSendData("LED Off\n\r", 10);
			LEDOff(RED);
		}
	}
}

void vTaskPrintTimer(void *pvParameters)
{
	while(1)
	{
		xUSBSendData(itoa(GetFastCounterValue(), 12), 10);
		xUSBSendData("\n\r", 2);
		vTaskDelay(1000);
	}
}
