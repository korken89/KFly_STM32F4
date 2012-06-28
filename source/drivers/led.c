#include "led.h"

/* Holders for the PIN typdefs */
GPIO_TypeDef *	GPIO_PORT[LEDn]	= {LED0_GPIO_PORT, LED1_GPIO_PORT};
const uint16_t 	GPIO_PIN[LEDn] 	= {LED0_PIN, LED1_PIN};
const uint32_t 	GPIO_CLK[LEDn] 	= {LED0_GPIO_CLK, LED1_GPIO_CLK};

void LEDInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(LED0_GPIO_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* Configure the GPIO_LED pins */
  GPIO_InitStructure.GPIO_Pin = LED0_PIN|LED1_PIN;
  GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
}

void LEDToggle(LED_TypeDef LED)
{
	GPIO_PORT[LED]->ODR ^= GPIO_PIN[LED];
}

void LEDOn(LED_TypeDef LED)
{
	GPIO_PORT[LED]->BSRRL = GPIO_PIN[LED];
}

void LEDOff(LED_TypeDef LED)
{
	GPIO_PORT[LED]->BSRRH = GPIO_PIN[LED];
}
