/* *
 *
 *
 * */

/* Includes */
#include "spi.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void SPI1Init(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;

  /* Enable clock for used IO pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Enable peripheral clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  /* Configure pins used by SPI1
   * PA4 = CS
   * PA5 = SCK
   * PA6 = MISO
   * PA7 = MOSI
   */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect SPI1 pins to SPI alternate function */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  /* Configure the chip select pin */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  SPI1_CS_HIGH();

  /* Configure the SPI as Master, Full Duplex */
  SPI_InitStructure.SPI_Direction           = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode                = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize            = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL                = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA                = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS                 = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler   = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit            = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial       = 7;
  SPI_Init(SPI1, &SPI_InitStructure); 

  SPI_Cmd(SPI1, ENABLE);
}

 /**
  * @brief      Reads a byte from the SPI.
  * 
  * @param      SPIx Which SPI to read from.
  * @return     Return the byte read.
  */
uint8_t SPI_ReadBytePolling(SPI_TypeDef *SPIx)
{
  return SPI_SendBytePolling(SPI_DUMMY_BYTE, SPIx);
}

 /**
  * @brief          Sends a byte through the SPI interface and return the byte received from the SPI.
  * 
  * @param byte     Byte to send.
  * @param SPIx     Which SPI to read from.
  * 
  * @return [description]
  */
uint8_t SPI_SendBytePolling(uint8_t byte, SPI_TypeDef *SPIx)
{
  /* Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI peripheral */
  SPI_I2S_SendData(SPIx, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPIx);
}