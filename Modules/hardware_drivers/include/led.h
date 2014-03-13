#ifndef __LED_H
#define __LED_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */

/* Scheduler includes. */

/* KFly includes */

/* Includes */


#define LEDn				2

#define LED0_PIN			GPIO_Pin_0
#define LED0_GPIO_PORT		GPIOC
#define LED0_GPIO_CLK		RCC_AHB1Periph_GPIOC

#define LED1_PIN			GPIO_Pin_1
#define LED1_GPIO_PORT		GPIOC
#define LED1_GPIO_CLK		RCC_AHB1Periph_GPIOC

typedef enum
{
	LED_GREEN = 1,
	LED_RED = 0
} LED_TypeDef;

void LEDInit(void);
void LEDToggle(LED_TypeDef);
void LEDOn(LED_TypeDef);
void LEDOff(LED_TypeDef);

#endif
