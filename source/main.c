#include "main.h"
#include <stdlib.h>

__attribute__((section(".sw_version"))) __I char build_version[] = KFLY_VERSION;
USB_OTG_CORE_HANDLE USB_OTG_dev;

volatile float testMatInv[6][6] = {{4.0736184f, 1.3924911f, 4.7858347f, 3.9610366f, 3.3936758f, 3.5302304f}, {4.5289597f, 2.7344076f, 2.4268782f, 4.7974621f, 3.7887007f, 0.15916423f}, {0.63493408f, 4.7875342f, 4.0014023f, 3.2787035f, 3.7156623f, 1.3846149f}, {4.5668793f, 4.8244427f, 0.70943169f, 0.17855839f, 1.9611351f, 0.23085695f}, {3.1617962f, 0.78806541f, 2.1088064f, 4.2456465f, 3.2773895f, 0.48565891f}, {0.48770202f, 4.8529639f, 4.5786776f, 4.6699662f, 0.85593344f, 4.1172891f}};
volatile float time_calc;

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
		InvertMatrix(&testMatInv[0][0]);
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

void vTaskPrintTimer(void *pvParameters)
{
	extern volatile uint8_t dataholder;
	uint8_t data[14];
	uint8_t send = MPU6050_RA_ACCEL_XOUT_H;
	I2C_MASTER_SETUP_Type Setup;

	uint8_t msg[] = {0xa6, 0x01, 0x00, CRC8(msg, 3), 0xaa, 0xbb, (uint8_t)(CRC16(msg,6)>>8), (uint8_t)(CRC16(msg,6))};

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
