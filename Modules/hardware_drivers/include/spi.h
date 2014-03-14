#ifndef __SPI_H
#define __SPI_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"

/* Scheduler includes */

/* KFly includes */

/* Includes */

/* Defines */
#define SPI_DUMMY_BYTE		0xFF

/* Macros */
#define SPI1_RF_CS_LOW()		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI1_RF_CS_HIGH()		GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define SPI1_FLASH_CS_LOW()		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI1_FLASH_CS_HIGH()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

/* Typedefs */

/* Global variable defines */

/* Global function defines */
void SPI1Init(void);
uint8_t SPI_ReadBytePolling(SPI_TypeDef *sSPI);
uint8_t SPI_SendBytePolling(uint8_t byte, SPI_TypeDef *sSPI);

#endif
