#include "main.h"
#include "stm32f4xx.h"

#define LED0_PIN                         GPIO_Pin_0
#define LED0_GPIO_PORT                   GPIOB
#define LED0_GPIO_CLK                    RCC_AHB1Periph_GPIOB

#define LED1_PIN                         GPIO_Pin_1
#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK                    RCC_AHB1Periph_GPIOB


USB_OTG_CORE_HANDLE USB_OTG_dev;

void LEDInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(LED0_GPIO_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = LED0_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LED1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
}

void LEDOn(void)
{
	LED0_GPIO_PORT->BSRRL = LED0_PIN;
	LED1_GPIO_PORT->BSRRL = LED1_PIN;
}

void LEDOff(void)
{
	LED0_GPIO_PORT->BSRRH = LED0_PIN;
	LED1_GPIO_PORT->BSRRH = LED1_PIN;
}

void main(void)
{
	/*STM32F4_Discovery_LEDInit(LED3);
	STM32F4_Discovery_LEDInit(LED4);
	STM32F4_Discovery_LEDInit(LED5);
	STM32F4_Discovery_LEDInit(LED6);
	STM32F4_Discovery_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

	STM32F4_Discovery_LEDOn(LED3);
	*/
	//for(volatile unsigned int i = 0; i < 0xFFFFFFFF; i++);

	//LEDInit();

	/*USBD_Init(&USB_OTG_dev,
		#ifdef USE_USB_OTG_HS
		  USB_OTG_HS_CORE_ID,
		#else
		  USB_OTG_FS_CORE_ID,
		#endif
		  &USR_desc,
		  &USBD_CDC_cb,
		  &USR_cb);
*/
	while (1)
	{
		for(volatile unsigned int i = 0; i < 0xFFFFFFFF; i++);

		//STM32F4_Discovery_LEDToggle(LED4);
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
