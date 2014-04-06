#include "main.h"
#include <stdlib.h>
#include "circularbuffer.h"
#include "statemachine_generators.h"
#include "led.h"
#include "ext_input.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;
static uint8_t DMA_buffer[32];
static uint8_t DMA_buffer2[32];
static uint8_t DMA_transmit[100];

void vTaskTest(void *pvParameters);
uint32_t itoa(int num, char *buf);

void main(void)
{
	/* Give the debugger time to halt the processor ~1s delay */
	for (volatile uint32_t i = 0; i < 0xFFFF; i++);

	/* *
	 *
	 * Set interrupt priority to 4 priority bits and 0 sub-priority bits.
	 *
	 * */

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

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
	 * RC Inputs init.
	 * Initializes and sets up the RC Inputs.
	 *
	 * */
	InputInit();

	/* *
	 *
	 * SPI1 init.
	 * Initializes and sets up SPI1.
	 *
	 * */
	SPI1Init();

	/* *
	 *
	 * External Flash init.
	 * Initializes and sets up the External Flash.
	 *
	 * */
	ExternalFlashInit();

	/* *
	 *
	 * USB receive queue init
	 *
	 * */
	vUSBQueueInit();

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

	/* *
	 *
	 * FastCounter init.
	 * Initializes and sets up a timer as a 1MHz counter for timing.
	 *
	 * */
	//FastCounterInit();

	/* *
	 *
	 * Initializes the I2C-bus.
	 *
	 * */
	SensorBusInit();

	/* *
	 *
	 * Initialize all sensors
	 *
	 * */
	MPU6050Init();
	HMC5983Init();

	/* *
	 *
	 * Initializes the sensor interrupts.
	 *
	 * */
	SensorsInterruptReadInit();

	/* *
	 *
	 * Estimation init.
	 *
	 * */
	EstimationInit();

	/* *
	 *
	 * Control init.
	 *
	 * */
	vInitControl();

	/* *
	 *
	 * Start the serial communication manager.
	 *
	 * */
	vSerialManagerInit();

	xTaskCreate(vTaskTest,
				"TestTask",
				256,
				0,
				tskSerialManagerPRIORITY,
			    0);

	/* *
	 *
	 * Start the scheduler.
	 *
	 * */
	vTaskStartScheduler();

	/* We only get here if there was insufficient memory to start the Scheduler */

	while(1);
}

void vTaskTest(void *pvParameters)
{
	while(1)
	{
		vTaskDelay(500);
		LEDToggle(LED_GREEN);
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
