#ifndef __UART_H
#define __UART_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void AUX1Init(uint32_t);
void USART3_IRQHandler(void);
void DMA_Configuration(uint8_t *, uint8_t *, uint16_t);
void DMA1_Stream2_IRQHandler(void);

#endif
