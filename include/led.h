#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

#define LEDn				2

#define LED0_PIN			GPIO_Pin_0
#define LED0_GPIO_PORT		GPIOB
#define LED0_GPIO_CLK		RCC_AHB1Periph_GPIOB

#define LED1_PIN			GPIO_Pin_1
#define LED1_GPIO_PORT		GPIOB
#define LED1_GPIO_CLK		RCC_AHB1Periph_GPIOB

typedef enum
{
  RED = 0,
  GREEN = 1
} LED_TypeDef;

void LEDInit(void);
void LEDToggle(LED_TypeDef);
void LEDOn(LED_TypeDef);
void LEDOff(LED_TypeDef);

#endif
