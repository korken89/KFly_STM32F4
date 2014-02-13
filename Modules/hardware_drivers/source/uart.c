/* *
 *
 *
 * */

/* Includes */
#include "uart.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* Private function defines */

void AUX1Init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable peripheral clock for USART3 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* GPIOC clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Enable the USART3 RX DMA Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TX/RX */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_10 | GPIO_Pin_11);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Enable USART AF */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	/* *
	 * Fill in the structure.
	 * 	   Word length: 8 bits
	 * 		 Stop bits: 1 bit
	 * 			Parity: None
	 * HW Flow Control: None
	 * 			  Mode: Receive and Transmit
	 * */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx);
	USART_Init(USART3, &USART_InitStructure);

	/* Setup USART Interrupt */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART 3 */
	USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void)
{
	/* *
	 * To catch the RXNE interrupt and handle DMA timeout, special care have to be taken.
	 * The DMA clears the interrupt bit when reading and hence it is impossible so read
	 * the RXNE bit to see if it has happened. The interrupt must be configured so only
	 * RXNE generates an interrupt
	 * */

	/* Disable the RXNE interrupt */
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);

	/* Start the DMA timeout timer */




	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, 'S');
}

void DMA_Configuration(uint8_t *buffer, uint8_t *buffer2, uint16_t size)
{
  DMA_InitTypeDef  DMA_InitStructure;

  DMA_DeInit(DMA1_Stream1);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // Receive
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer;
  DMA_InitStructure.DMA_BufferSize = size;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA1_Stream1, &DMA_InitStructure);


  /* Configure Double Buffer */
  DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)buffer2, DMA_Memory_1);

  /* Enable the USART RX DMA request */
  USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);

  /* Enable the DMA RX Stream */
  DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);
  DMA_Cmd(DMA1_Stream1, ENABLE);
}

void DMA1_Stream1_IRQHandler(void)
{
  /* Test on DMA Stream Transfer Complete interrupt */
  if (DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);

    if (DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0)
    {
    	/* Buffer full, enable RXNE interrupt of the USART to detect DMA timeout */
    	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);



    	USART_SendData(USART3, 'C');
    	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    	USART_SendData(USART3, '0');
    }
    else
    {
    	/* Buffer full, enable RXNE interrupt of the USART to detect DMA timeout */
    	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);



    	USART_SendData(USART3, 'C');
    	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    	USART_SendData(USART3, '1');
    }
  }

  /* Test on DMA Stream Half Transfer interrupt */
  if (DMA_GetITStatus(DMA1_Stream1, DMA_IT_HTIF1))
  {
    /* Clear DMA Stream Half Transfer interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_HTIF1);

    if (DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0)
    {
    	USART_SendData(USART3, 'H');
    	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    	USART_SendData(USART3, '1');
    }
    else
    {
    	USART_SendData(USART3, 'H');
    	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    	USART_SendData(USART3, '0');
    }
  }
}
