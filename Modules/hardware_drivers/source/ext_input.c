/* *
 *
 * Hardware Abstraction Layer for External RC Inputs
 *
 * */

#include "ext_input.h"

#define CAPTURE_TIMER_RATE 		1000000

/* Global variable defines */

/* Private variable defines */
Raw_External_Input_Type raw_rc_input;

/* Private function defines */


/* Private external functions */

/*
Input connections:
	Control In 1: TIM9_CH3 (PA2)
	Control In 2: TIM12_CH2 (PB15)
	Control In 3: TIM12_CH1 (PB14)
	Control In 4: TIM3_CH4 (PB1)
	Control In 5: TIM3_CH3 (PB0)
	Control In 6: TIM9_CH4 (PA3)
*/
void Input_CPPM_RSSI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	/* If timers are running: Disable Timers */
	TIM_Cmd(TIM3, DISABLE);
	TIM_Cmd(TIM9, DISABLE);
	TIM_Cmd(TIM12, DISABLE);

	/* Disable interrupts */
	TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC3, DISABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC4, DISABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC1, DISABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, DISABLE);


	/* TIM clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);


	/* Inputs are on GPIOA and GPIOB */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	  

	/* GPIO configuration */
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;

	/* GPIOA configuration */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB configuration */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* Connect TIM pins to AF */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM12);


	/* Enable the TIM global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_Init(&NVIC_InitStructure);


	/* Setup TIM time base */
	TIM_TimeBaseStructure.TIM_Period 			= 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler 		= ((SystemCoreClock / 2) / CAPTURE_TIMER_RATE) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / CAPTURE_TIMER_RATE) - 1;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);	/* Run on APB2, different prescaler */
	

	/* Setup Input Capture */
	TIM_ICInitStructure.TIM_ICPolarity 		= TIM_ICPolarity_BothEdge;
	TIM_ICInitStructure.TIM_ICSelection 	= TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler 	= TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter 		= 0x0;

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);


	/* Enable interrupts */
	TIM_ITConfig(TIM9, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, ENABLE);


	/* Clear interrupts before enabling the timers */
	TIM_ClearFlag(TIM9, TIM_FLAG_CC3);
	TIM_ClearFlag(TIM9, TIM_FLAG_CC4);
	TIM_ClearFlag(TIM12, TIM_FLAG_CC1);
	TIM_ClearFlag(TIM12, TIM_FLAG_CC2);


	/* Enable Timers */
	TIM_Cmd(TIM9, ENABLE);
	TIM_Cmd(TIM12, ENABLE);
}

void Input_PWM_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	/* If timers are running: Disable Timers */
	TIM_Cmd(TIM3, DISABLE);
	TIM_Cmd(TIM9, DISABLE);
	TIM_Cmd(TIM12, DISABLE);

	/* Disable interrupts */
	TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC3, DISABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC4, DISABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC1, DISABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, DISABLE);


	/* TIM clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);


	/* Inputs are on GPIOA and GPIOB */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	  

	/* GPIO configuration */
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;

	/* GPIOA configuration */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB configuration */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* Connect TIM pins to AF */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM12);


	/* Enable the TIM global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_Init(&NVIC_InitStructure);


	/* Setup TIM time base */
	TIM_TimeBaseStructure.TIM_Period 			= 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler 		= ((SystemCoreClock / 2) / CAPTURE_TIMER_RATE) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler 		= (SystemCoreClock / CAPTURE_TIMER_RATE) - 1;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);	/* Run on APB2, different prescaler */
	

	/* Setup Input Capture */
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);


	/* Enable interrupts */
	TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, ENABLE);


	/* Clear interrupts before enabling the timers */
	TIM_ClearFlag(TIM3, TIM_FLAG_CC3);
	TIM_ClearFlag(TIM3, TIM_FLAG_CC4);
	TIM_ClearFlag(TIM9, TIM_FLAG_CC3);
	TIM_ClearFlag(TIM9, TIM_FLAG_CC4);
	TIM_ClearFlag(TIM12, TIM_FLAG_CC1);
	TIM_ClearFlag(TIM12, TIM_FLAG_CC2);


	/* Enable Timers */
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM9, ENABLE);
	TIM_Cmd(TIM12, ENABLE);
}


