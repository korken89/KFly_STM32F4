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
void vStartDMATimeout(TIM_TypeDef *, uint16_t, uint16_t);

void USART3Init(uint32_t baudrate)
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART3 TX DMA Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART 3 */
	USART_Cmd(USART3, ENABLE);
}

void USART_putc(USART_TypeDef *USARTx, uint8_t ch)
{
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(USARTx, ch);
}

void USART_puts(USART_TypeDef *USARTx, uint8_t *msg)
{
	while (*msg)
	{
		USART_putc(USARTx, *(msg++));
	}
}

void DMA_Receive_Configuration(uint8_t *buffer, uint8_t *buffer2, uint16_t size)
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

  /* Start the DMA Timeout */
  DMATimeoutInit();

  DMA_Cmd(DMA1_Stream1, ENABLE);
}

void DMA_Transmit_Configuration(void)
{
  DMA_InitTypeDef  DMA_InitStructure;

  DMA_DeInit(DMA1_Stream3);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; // Transmit
  DMA_InitStructure.DMA_Memory0BaseAddr = 0x00000000;
  DMA_InitStructure.DMA_BufferSize = 0;
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

  DMA_Init(DMA1_Stream3, &DMA_InitStructure);

  /* Enable the USART TX DMA request */
  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);

  DMA_Cmd(DMA1_Stream3, ENABLE);
}

void DMA_Transmit_Buffer(DMA_Stream_TypeDef *DMAx_Streamy, uint8_t *buffer, uint16_t size)
{
	/* Load the buffer and the buffer size */
	DMAx_Streamy->NDTR = size;
	DMAx_Streamy->M0AR = (uint32_t)buffer;

	/* Initialize DMA transfer */
	DMA_Cmd(DMAx_Streamy, ENABLE);
}

void DMATimeoutInit(void)
{
	const uint16_t TIMER_RATE = 10000;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	uint16_t PrescalerValue = ((SystemCoreClock /2) / TIMER_RATE) - 1;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* Output compare configuration */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* Enable interrupts */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ITConfig(TIM2, (TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4), DISABLE);

    TIM_Cmd(TIM2, ENABLE);
}

void vStartDMATimeout(TIM_TypeDef *TIMx, uint16_t TIM_IT, uint16_t wait_ticks)
{
	/* Calculate the new Compare value */
	uint16_t NewCompare = (uint16_t)TIMx->CNT;
	NewCompare += wait_ticks;

	/* Start the DMA timeout timer */
	TIM_SetCompare1(TIMx, NewCompare);
	TIM_ClearITPendingBit(TIMx, TIM_IT); /* For some reason the pending bit must be cleared */
	TIM_ITConfig(TIMx, TIM_IT, ENABLE);
}

void USART3_IRQHandler(void)
{
	/* *
	 * To catch the RXNE (receive not empty) interrupt and handle DMA timeout, special
	 * care have to be taken. The DMA clears the interrupt bit when reading and hence it
	 * is impossible so read the RXNE bit to see if it has happened. The interrupt must
	 * be configured so only RXNE generates an interrupt.
	 * */

	/* Disable the RXNE interrupt */
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);

	/* Start the DMA timeout interrupt */
	vStartDMATimeout(TIM2, TIM_IT_CC1, 30000);
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

    	/* TODO:
    	 * Add buffer0 read out.
    	 * */
    }
    else
    {
    	/* Buffer full, enable RXNE interrupt of the USART to detect DMA timeout */
    	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    	/* TODO:
    	 * Add buffer1 read out.
    	 * */
    }
  }
}

void DMA1_Stream3_IRQHandler(void)
{
  /* Test on DMA Stream Transfer Complete interrupt */
  if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);

  }
}

void TIM2_IRQHandler(void)
{
	/* Check if the Compare interrupt has happened,
	 * this means a DMA timeout has occurred. */
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1))
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);

		/* TODO:
    	 * Add DMA timeout code.
    	 * */
	}
}
