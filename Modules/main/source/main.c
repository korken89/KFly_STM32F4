#include "main.h"
#include <stdlib.h>

__attribute__((section(".sw_version"))) __I char build_version[] = KFLY_VERSION;
USB_OTG_CORE_HANDLE USB_OTG_dev;
static uint8_t DMA_buffer[32];
static uint8_t DMA_buffer2[32];

void main(void)
{
	/* Give the debugger time to halt the processor ~1s delay */
	for (volatile uint32_t i = 0; i < 0xFFFFF; i++);

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



	AUX1Init(115200);
	DMA_Configuration(DMA_buffer, DMA_buffer2, 32);

	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); // Wait for Empty
	USART_SendData(USART3, '*');

	while(1)
	{
		/*while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

		uint8_t x = USART_ReceiveData(USART3);

		USART_SendData(USART3, x);
		LEDToggle(LED_GREEN);
		LEDToggle(LED_RED);*/
	}
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
	 * Initialize all sensors
	 *
	 * */
	MPU6050Init();
	HMC5883LInit();

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
	 * Start the serial communication manager.
	 *
	 * */
	vSerialManagerInit();

	/* *
	 *
	 * Start the scheduler.
	 *
	 * */
	vTaskStartScheduler();

	/* We only get here if there was insufficient memory to start the Scheduler */

	while(1);
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
