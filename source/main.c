#include "main.h"
#include <stdlib.h>

#ifndef DATE
	#define DATE "no timestamp"
#endif

#define MAINVERSION	 	"1"
#define SUBVERSION 		"23"

#define KFLY_VERSION "KFly v" MAINVERSION "." SUBVERSION " (20" DATE ")\0"

__attribute__((section(".sw_version"))) volatile const char build_version[] = KFLY_VERSION;
USB_OTG_CORE_HANDLE USB_OTG_dev;

volatile float testMatInv[12][12] = {{3.4741431f, 3.5468242f, 3.7563353f, 4.0714241f, 4.5859683f, 2.8441183f, 3.4460725f, 0.53326385f, 0.90923514f, 2.0090402f, 1.688597f, 2.876043f}, {1.5854974f, 3.7734334f, 1.2754756f, 1.2176248f, 1.4291951f, 2.3469532f, 3.740758f, 4.8094904f, 1.3190146f, 0.37983346f, 4.5002692f, 0.29889771f}, {4.7511102f, 1.3801254f, 2.5297853f, 4.6463181f, 3.7860011f, 0.059510348f, 2.252708f, 0.023171121f, 0.7276949f, 1.1995808f, 1.8462339f, 1.1738996f}, {0.1722304f, 3.3985134f, 3.4953836f, 1.7499188f, 3.7686455f, 1.6856132f, 0.41910689f, 3.8745523f, 0.68034279f, 0.61659467f, 0.55601378f, 1.7657929f}, {2.1937218f, 3.27549f, 4.4545163f, 0.98297625f, 1.9022292f, 0.81091154f, 1.1448848f, 4.0865161f, 4.346461f, 0.91953894f, 3.9012603f, 4.1059702f}, {1.9077923f, 0.81305868f, 4.7964571f, 1.2554193f, 2.8391082f, 3.9714227f, 4.5666868f, 4.3434735f, 2.8985229f, 1.1997626f, 1.9486942f, 0.077017188f}, {3.8275839f, 0.59498841f, 2.7360776f, 3.0802234f, 0.37927145f, 1.5560752f, 0.76189009f, 0.42217923f, 2.749301f, 2.0863353f, 1.2084564f, 0.21511901f}, {3.9759995f, 2.4918203f, 0.69312221f, 2.3664442f, 0.26975059f, 2.6426657f, 4.1290849f, 1.9989132f, 0.72477399f, 0.24827215f, 2.0195607f, 0.84495015f}, {0.93436302f, 4.7987198f, 0.74647003f, 1.7582975f, 2.6539878f, 0.82824365f, 2.6917122f, 1.299352f, 4.2651556f, 4.5135805f, 0.48227263f, 3.2455774f}, {2.448822f, 1.7019286f, 1.2875413f, 4.1541431f, 3.8958362f, 3.0099097f, 4.9806736f, 4.0003424f, 3.1102757f, 4.7239359f, 0.65986646f, 3.6586119f}, {2.227931f, 2.9263388f, 4.2035863f, 2.9263205f, 4.6700534f, 1.3148564f, 0.39087764f, 2.1570691f, 1.7547619f, 2.4543205f, 4.710253f, 3.2387298f}, {3.2315651f, 1.1190597f, 1.2714109f, 2.748618f, 0.64953104f, 3.2703955f, 2.2133913f, 4.553238f, 2.5662477f, 2.4462632f, 4.7806727f, 2.2546185f}};
volatile float time_calc;

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

	//for (volatile uint32_t i = 0; i < 0xFFFFFF; i++);
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
	 * Initialize all sensors
	 * */
	InitMPU6050();

	time_calc = (float)GetFastCounterValue();
	{
	for (int i = 0; i < 10000; i++)
		InvertMatrix(&testMatInv[0][0], 12);
	}
	time_calc = (float)GetFastCounterValue() - time_calc;
	LEDOn(RED);

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

	vInitSerialManager();

	/*xTaskCreate(vTaskCode,
				"MISC",
				256,
				0,
				tskIDLE_PRIORITY + 1,
		    	0);*/

	xTaskCreate(vTaskPrintTimer,
				"TIMER",
				256,
				0,
				tskIDLE_PRIORITY + 1,
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
		xQueueReceive(xUSBQueue.xUSBQueueHandle, &text, portMAX_DELAY);

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
	extern volatile uint8_t dataholder;
	uint8_t data[14];
	uint8_t send = MPU6050_RA_ACCEL_XOUT_H;
	I2C_MASTER_SETUP_Type Setup;

	uint8_t msg[] = {0xa6, 0x01, 0x00, CRC8(msg, 3), 0xaa, 0xbb, 0, 0};
	uint16_t crc = CRC16(msg,6);
	msg[6] = (uint8_t)(crc>>8);
	msg[7] = (uint8_t)(crc);

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = data;
	Setup.RX_Length = 14;
	Setup.Retransmissions_Max = 0;
	Setup.Callback = NULL;

	while(1)
	{
		vTaskDelay(5000);
		//GetMPU6050ID((uint8_t *)&dataholder);
		GetHMC5883LID(data);
		//xUSBSendData(msg, 8);
		//I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_INTERRUPT);
	}
}
