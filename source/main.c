#include "main.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;

extern uint16_t cdc_DataTx (uint8_t* Buf, uint32_t Len);

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


	/* *
	 *
	 *
	 *
	 *
	 * */


	char text = 'a';

	while (1)
	{
		for(volatile unsigned int i = 0; i < 8000000; i++);

		cdc_DataTx(&text, 1);

		if (text >= 'z')
		{
			text = 'a';
			cdc_DataTx("\n\r", 2);
		}
		else
			text++;
	}
}
