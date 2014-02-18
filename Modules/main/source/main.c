#include "main.h"
#include <stdlib.h>

__attribute__((section(".sw_version"))) __I char build_version[] = KFLY_VERSION;
USB_OTG_CORE_HANDLE USB_OTG_dev;
static uint8_t DMA_buffer[32];
static uint8_t DMA_buffer2[32];
static uint8_t DMA_transmit[100];

static inline void CircularBuffer_DMATransmit(DMA_Stream_TypeDef *DMAx_Streamy, Circular_Buffer_Type *Cbuff, const uint32_t buffer_size)
{
	uint32_t count, head, to_top;

	count = CircularBuffer_SpaceLeft(Cbuff, buffer_size);
	head = (Cbuff->start + Cbuff->count) % buffer_size;
	to_top = buffer_size - head;

	if (count <= to_top)
	{	/* Head is larger than tail, no wrap around */

		DMA_Transmit_Buffer(DMAx_Streamy, &(Cbuff->buffer[Cbuff->start]), count);

		/* Set tail to equal head since we are now at the same position */
		Cbuff->start += count;
		Cbuff->count -= count;
	}
	else
	{	/* Head is smaller than tail, wrap around will occur.
		 * Transmit the data to the end of the buffer.
		 * */
		DMA_Transmit_Buffer(DMAx_Streamy, &(Cbuff->buffer[Cbuff->start]), to_top);

		/* Set tail to zero since we are now at the start of the buffer */
		Cbuff->start = 0;
		Cbuff->count -= to_top;
	}
}

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

	static uint8_t buf1[] = "This is a short text to test the DMA transfers via USART...\r\n";
	Circular_Buffer_Type CBuff;
	CircularBuffer_Init(&CBuff, DMA_transmit);

	USART3Init(115200);
	DMA_Receive_Configuration(DMA_buffer, DMA_buffer2, 32);
	DMA_Transmit_Configuration();

	CircularBuffer_WriteChunk(&CBuff, buf1, 61, 100);
	CircularBuffer_DMATransmit(DMA1_Stream3, &CBuff, 100);

	for (volatile uint32_t i = 0; i < 0xFFFFFF; i++);
	CircularBuffer_WriteChunk(&CBuff, buf1, 61, 100);
	CircularBuffer_DMATransmit(DMA1_Stream3, &CBuff, 100);

	for (volatile uint32_t i = 0; i < 0xFFFFFF; i++);
	CircularBuffer_DMATransmit(DMA1_Stream3, &CBuff, 100);
	//DMA_Transmit_Buffer(DMA1_Stream3, buf1, 61);


	while(1);

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
	//FastCounterInit();

	/* *
	 *
	 * Initializes the I2C-bus.
	 *
	 * */
	//SensorBusInit();

	/* *
	 *
	 * Initialize all sensors
	 *
	 * */
	//MPU6050Init();
	//HMC5883LInit();

	/* *
	 *
	 * Initializes the sensor interrupts.
	 *
	 * */
	//SensorsInterruptReadInit();

	/* *
	 *
	 * Estimation init.
	 *
	 * */
	//EstimationInit();

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
