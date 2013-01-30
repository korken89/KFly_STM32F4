#include "main.h"
#include <stdlib.h>

__attribute__((section(".sw_version"))) __I char build_version[] = KFLY_VERSION;
USB_OTG_CORE_HANDLE USB_OTG_dev;

void main(void)
{
	for (volatile uint32_t i = 0; i < 0xFFFFF; i++);
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
	 * PWM init.
	 * Initializes and sets up the PWMs.
	 *
	 * */
	PWMInit();

	/* *
	 *
	 * USB receive queue init
	 *
	 * */
	vUSBQueueInit();

	/* *
	 *
	 * FastCounter init.
	 * Initializes and sets up a timer as a 1MHz counter for timing.
	 *
	 * */
	FastCounterInit();

	/* *
	 *
	 * Initializes the I2C-bus.
	 *
	 * */
	SensorBusInit();

	/* *
	 *
	 * Initializes the sensor interrupts
	 *
	 * */
	EXTI_SensorInit();

	/* *
	 *
	 * Initialize all sensors
	 *
	 * */
	MPU6050Init();
	HMC5883LInit();

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

	vSerialManagerInit();

	xTaskCreate(vTaskPrintTimer,
				"TIMER",
				256,
				0,
				tskIDLE_PRIORITY + 1,
				0);

	vTaskStartScheduler();

	/* We only get here if there was insufficient memory to start the Scheduler */

	while(1);
}

void TestCallback(void)
{
	LEDToggle(LED_RED);
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
	Setup.Callback = TestCallback;

	while(1)
	{
		vTaskDelay(1000);
		//GetMPU6050ID((uint8_t *)&dataholder);
		//GetHMC5883LID(data);
		//xUSBSendData(msg, 8);
		I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_INTERRUPT);
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
