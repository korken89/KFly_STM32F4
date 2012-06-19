#include "main.h"
#include <stdlib.h>

USB_OTG_CORE_HANDLE USB_OTG_dev;

uint32_t itoa(int num, char *buf)
{
	if (num == 0)
	{
		buf[10] = '0';
		return 10;
	}

	int i;
	for(i = 10; (num && i); i--)
	{
		buf[i] = "0123456789"[num % 10];
		num /= 10;
	}

	return (i+1);
}

void ftoa(float num)
{

	const float log2_of_10 = 3.321928094887362f;
	char text[11] = {0};
	char buff[15] = {0};
	uint32_t id = 0;
	int exp;
	int flag1 = 0, flag2 = 0;

	if (num == 0.0f)
	{
		xUSBSendData("0.000000e0", 10);
		return;
	}

	if (num < 0.0f)
	{
		num = -num;
		flag1 = 1;
	}

	uint32_t i = *(uint32_t *)&num;
	exp = (int)(i >> 23);
	exp = (int)(exp) - 127;
	int mantissa = (i & 0xFFFFFF) | 0x800000;

	float f_exp = (float)exp;
	f_exp /= log2_of_10;	// To log10
	exp = (int)f_exp;

	float mul;

	if (exp > 0)
	{
		mul = 0.1f;
	}
	else if (exp< 0)
	{
		mul = 10.0f;
		flag2 = 1;
	}

	int cnt = abs(exp)+1;
	// Normalize
	while (cnt--)
	{
		num *= mul;
	}




	int whole = (int)num;
	float whole_f = (float)whole;
	num -= whole_f;
	num *= 1000000.0f; // 6 decimals of precision
	int dec = (int)num;

	if (flag1)
		xUSBSendData("-", 1);

	id = itoa((uint32_t)whole, text);
	xUSBSendData(&text[id], 11-id);

	xUSBSendData(".", 1);

	id = itoa((uint32_t)dec, text);

	if (11-id < 6)
		for (int i = 0; i < 6+id-11; i++)
			xUSBSendData("0", 1);

	xUSBSendData(&text[id], 11-id);

	xUSBSendData("e", 1);

	if (flag2)
	{
		xUSBSendData("-", 1);
	}
	else
		xUSBSendData("+", 1);

	id = itoa((uint32_t)abs(exp)+1, text);
	xUSBSendData(&text[id], 11-id);
}

void main(void)
{
	/* *
	 *
	 * Initialization of peripherals and I/O-ports
	 *
	 * */


	/* *
	 *
	 * LED init.
	 * Initializes and sets up the port for the LEDs as outputs.
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
	 * Initializes and sets up a timer as a 1MHz counter for timing.
	 *
	 * */
	InitFastCounter();

	/* *
	 *
	 * Initializes the I2C-bus.
	 *
	 * */
	InitSensorBus();

	/* *
	 *
	 * 	USB init.
	 * 	Running USB Full Speed as Virtual COM Port via the CDC interface
	 * 	Shows as ttyACMxx in Linux and COMxx in Windows.
	 * 	Linux version does not need a driver but Windows version uses STM serial driver.
	 *
	 * */
	USBD_Init(	&USB_OTG_dev,
				USB_OTG_FS_CORE_ID,
				&USR_desc,
				&USBD_CDC_cb,
				&USR_cb);

	xTaskCreate(vTaskCode,
				"MISC",
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

	while(1);
}

void vTaskCode(void *pvParameters)
{
	char text;

	while(1)
	{
		xQueueReceive(xUSBQueueHandle, &text, portMAX_DELAY);

		if (text == 'a')
		{
			xUSBSendData("LED On\n\r", 8);
			LEDOn(RED);
		}

		else if (text == 's')
		{
			xUSBSendData("LED Off\n\r", 9);
			LEDOff(RED);
		}
	}
}

void vTaskPrintTimer(void *pvParameters)
{
	uint8_t data;
	while(1)
	{
		GetMUP6050ID(&data);
		vTaskDelay(5000);
	}
}
