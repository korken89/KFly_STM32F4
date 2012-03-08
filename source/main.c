#include "main.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;

void main(void)
{
	STM32F4_Discovery_LEDInit(LED3);
	STM32F4_Discovery_LEDInit(LED4);
	STM32F4_Discovery_LEDInit(LED5);
	STM32F4_Discovery_LEDInit(LED6);
	STM32F4_Discovery_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

	STM32F4_Discovery_LEDOn(LED3);

	for(volatile unsigned int i = 0; i < 0xFFFFF; i++);

	USBD_Init(&USB_OTG_dev,
		#ifdef USE_USB_OTG_HS
		  USB_OTG_HS_CORE_ID,
		#else
		  USB_OTG_FS_CORE_ID,
		#endif
		  &USR_desc,
		  &USBD_CDC_cb,
		  &USR_cb);

	while (1)
	{
		for(volatile unsigned int i = 0; i < 0xFFFFF; i++);

		STM32F4_Discovery_LEDToggle(LED4);
	}
}

void memset(int *dst, int c, int size)
{
	while(size--) *(dst++) = c;
}
void memcpy(int *dst, int *src, int size)
{
	while(size--) *(dst++) = *(src++);
}
