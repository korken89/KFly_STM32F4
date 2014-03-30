/* *
 *
 * Hardware Abstraction Layer for External RC Inputs
 *
 * */

#include "ext_input.h"
#include "FreeRTOSConfig.h"

/* Global variable defines */

/* Private variable defines */
Raw_External_Input_Type raw_rc_input;

/* Private function defines */


/* Private external functions */

/*
Input connections:
	Control In 1:  TIM9_CH1   (PA2)
	Control In 2:  TIM12_CH2  (PB15)
	Control In 3:  TIM12_CH1  (PB14)
	Control In 4:  TIM3_CH4   (PB1)
	Control In 5:  TIM3_CH3   (PB0)
	Control In 6:  TIM9_CH2   (PA3)
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
	TIM_ITConfig(TIM9, TIM_IT_CC1, DISABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC2, DISABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC1, DISABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, DISABLE);


	/* Set input mode */
	raw_rc_input.cppm_mode = TRUE;


	/* TIM clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);


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
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM pins to AF */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM12);


	/* Enable the TIM global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= configMAX_SYSCALL_INTERRUPT_PRIORITY + 2;
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
	TIM_ICInitStructure.TIM_ICPolarity 		= TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection 	= TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler 	= TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter 		= 0x0;

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);

	/* Set up PWM Input Capture */
	TIM_ICInitStructure.TIM_ICPolarity 	= TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_Channel 	= TIM_Channel_2;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);
	TIM_PWMIConfig(TIM12, &TIM_ICInitStructure);

	/* Select the TIM12 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM12, TIM_TS_TI2FP2);

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM12, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM12, TIM_MasterSlaveMode_Enable);


	/* Enable interrupts */
	TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, ENABLE);


	/* Clear interrupts before enabling the timers */
	TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM12, TIM_IT_CC2);


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


	/* Set input mode */
	raw_rc_input.cppm_mode = FALSE;


	/* TIM clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);


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
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	/* Connect TIM pins to AF */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM12);


	/* Enable the TIM global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= configMAX_SYSCALL_INTERRUPT_PRIORITY + 2;
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
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);	/* Runs on APB2, use different prescaler */
	

	/* Setup Input Capture */
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM9, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM12, &TIM_ICInitStructure);


	/* Enable interrupts */
	TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM9, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM12, TIM_IT_CC2, ENABLE);


	/* Clear interrupts before enabling the timers */
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
	TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM9, TIM_IT_CC2);
	TIM_ClearITPendingBit(TIM12, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM12, TIM_IT_CC2);


	/* Enable Timers */
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM9, ENABLE);
	TIM_Cmd(TIM12, ENABLE);
}


Raw_External_Input_Type *ptrGetRawRCInput(void)
{
	return &raw_rc_input;
}

/**
 * @brief 			Processing of Input Capture events for the RC Input.
 * 
 * @param channel 	Which capture channel that fired
 * @param capture 	The capture value
 */
static void Process_InputCapture(Input_Capture_Channel channel, uint32_t capture)
{
	static uint32_t old_capture[6];	/* The last capture value */
	static uint16_t cppm_count = 0;	/* Current CPPM channel */
	uint16_t tmp;

	/* If CPPM mode */
	if (raw_rc_input.cppm_mode == TRUE)
	{
		if (channel == INPUT_CH1_CPPM)
		{	
			/* 	CPPM only needs 1 old value since it comes in as a serial stream.
		   	Plus check if the timer overflowed. */
		   	tmp = capture;

			if (capture > old_capture[0])		/* No overflow */
				capture = capture - old_capture[0];
			else if (capture < old_capture[0])	/* Timer overflow */
				capture = ((0xFFFF - old_capture[0]) + capture); 

			old_capture[0] = tmp;

			/*
			 * Decoding of CPPM
			 */
			if (raw_rc_input.active_connection == TRUE)
			{	
				/* If the capture is larger than the time for the SYNC, reset counter */
				if (capture > CPPM_SYNC_LIMIT_MIN)
				{
					cppm_count = 0;
				}
				else
				{
					/* If no sync has been detected */
					if (cppm_count >= MAX_NUMBER_OF_INPUTS)
					{
						raw_rc_input.active_connection = FALSE;
						return;
					}

					/* Write the capture value to the raw_rc_input structure */
					raw_rc_input.value[cppm_count] = capture;

					/* Increase the current CPPM channel */
					cppm_count++;
				}
			}
			else if ((capture > CPPM_SYNC_LIMIT_MIN) && (capture < CPPM_SYNC_LIMIT_MAX))
			{
				/* Looking for sync */
				cppm_count = 0;
				raw_rc_input.active_connection = TRUE;
			}

		}
		else if (channel == INPUT_CH2_RSSI) /* RSSI capture */
		{ 	
			/* Get the Input Capture value */
			raw_rc_input.rssi_frequency = CAPTURE_TIMER_RATE / capture;

			/* If there is valid data, save it else reset RSSI values */
			if (capture != 0)
			{
				raw_rc_input.rssi = TIM_GetCapture1(TIM12);
			}
			else
			{
				raw_rc_input.rssi = 0;
				raw_rc_input.rssi_frequency = 0;
			}
		}
	}

	/* If PWM mode */
	else
	{

	}
}


void TIM3_IRQHandler(void)
{
	/* Check which capture event that was triggered */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) == SET)
	{
		/* Clear interrupt flag */
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);

		/* Process the interrupt */
		Process_InputCapture(INPUT_CH5, TIM_GetCapture3(TIM3));
	}

	if (TIM_GetITStatus(TIM3, TIM_IT_CC4) == SET)
	{
		/* Clear interrupt flag */
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);

		/* Process the interrupt */
		Process_InputCapture(INPUT_CH4, TIM_GetCapture4(TIM3));
	}
}


void TIM1_BRK_TIM9_IRQHandler(void)
{
	/* Check which capture event that was triggered */
	if (TIM_GetITStatus(TIM9, TIM_IT_CC1) == SET)
	{
		/* Clear interrupt flag */
		TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);

		/* Process the interrupt */
		Process_InputCapture(INPUT_CH1_CPPM, TIM_GetCapture1(TIM9));
	}

	if (TIM_GetITStatus(TIM9, TIM_IT_CC2) == SET)
	{
		/* Clear interrupt flag */
		TIM_ClearITPendingBit(TIM9, TIM_IT_CC2);

		/* Process the interrupt */
		Process_InputCapture(INPUT_CH6, TIM_GetCapture2(TIM9));
	}
}


void TIM8_BRK_TIM12_IRQHandler(void)
{
	/* Check which capture event that was triggered */
	if (TIM_GetITStatus(TIM12, TIM_IT_CC1) == SET)
	{
		/* Clear interrupt flag */
		TIM_ClearITPendingBit(TIM12, TIM_IT_CC1);

		/* Process the interrupt */
		Process_InputCapture(INPUT_CH3, TIM_GetCapture1(TIM12));
	}

	if (TIM_GetITStatus(TIM12, TIM_IT_CC2) == SET)
	{
		/* Clear interrupt flag */
		TIM_ClearITPendingBit(TIM12, TIM_IT_CC2);

		/* Process the interrupt */
		Process_InputCapture(INPUT_CH2_RSSI, TIM_GetCapture2(TIM12));
	}
}
