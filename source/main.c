#include "main.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;

void main(void)
{
	//for(volatile unsigned int i = 0; i < 0xFFFFFFFF; i++);

	LEDInit();

	//LEDOn(GREEN);

	USBD_Init(&USB_OTG_dev,
		  USB_OTG_FS_CORE_ID,
		  &USR_desc,
		  &USBD_CDC_cb,
		  &USR_cb);

	//LEDOn(RED);

	while (1)
	{
		/*for(volatile unsigned int i = 0; i < 4000000; i++);
		LEDOn(GREEN);
		LEDOff(RED);

		for(volatile unsigned int i = 0; i < 4000000; i++);
		LEDOff(GREEN);
		LEDOn(RED);*/
	}
}
