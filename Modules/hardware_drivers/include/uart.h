#ifndef __UART_H
#define __UART_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void USART3Init(uint32_t);
void USART_putc(USART_TypeDef *, uint8_t);
void USART_puts(USART_TypeDef *, uint8_t *);
void DMA_Receive_Configuration(uint8_t *, uint8_t *, uint16_t);
void DMA_Transmit_Configuration(void);
void DMA_Transmit_Buffer(DMA_Stream_TypeDef *, uint8_t *, uint16_t);
void DMATimeoutInit(void);
void USART3_IRQHandler(void);
void DMA1_Stream1_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void);
void TIM2_IRQHandler(void);

#endif
