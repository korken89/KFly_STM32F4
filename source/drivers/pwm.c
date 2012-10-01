/* *
 *
 * Hardware Abstraction Layer for PWMs
 *
 * The PWM driver consists of three Timers; TIM3, 4 and 8.
 * With this setup they can have different period time so
 * both 50 Hz and 400 Hz signals can be produced.
 *
 * PWM Hz: 50 (5-10% duty) / 400 (40-80% duty)
 * Constant Counter method:
 * Counter rate: 1000000 ticks per second
 * Period 50 Hz: 20000 (1000-2000 CCR) gives 0.1% step size
 * Period 400 Hz: 2500 (1000-2000 CCR) gives 0.1% step size
 *
 * */

#include "pwm.h"

/* Global variable defines */

/* Private function defines */

/* Private external functions */

void PWMInit(void)
{
	/* TODO: Add PWM setup  */

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t PrescalerValue = ((SystemCoreClock /2) / TIMER_RATE) - 1;

	/* *
	 * Timer setup starts here!
	 * */

	/* TIM3/4/8 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	/* Outputs 1-4 are on GPIOB, 5-8 on GPIOC */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Fastest PWM is 400Hz, 2MHz speed is enough */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Set to Timer AF */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4); /* Output 4 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4); /* Output 3 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4); /* Output 2 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4); /* Output 1 */

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3); /* Output 8 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3); /* Output 7 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8); /* Output 6 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8); /* Output 5 */

	/* *
	 * PWM setup starts here!
	 * */

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = PEROID_50HZ; /* Start at 50 Hz PWM */
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	/* Apply time base configuration */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	PrescalerValue = ((SystemCoreClock) / TIMER_RATE) - 1; /* Runs on APB2 (84 MHz) */
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	/* PWM1 Mode configuration: Channel 1 */
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel 2 */
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel 3 */
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel 4 */
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

	/* Enable preload */
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_ARRPreloadConfig(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);

	/* Enable timers */
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM8, ENABLE);

	/* Set start PWM value at 1ms pulse width */
	TIM4->CCR1 = 2000; /* Output 4 */
	TIM4->CCR2 = 2000; /* Output 3 */
	TIM4->CCR3 = 2000; /* Output 2 */
	TIM4->CCR4 = 2000; /* Output 1 */
	TIM3->CCR1 = 2000; /* Output 8 */
	TIM3->CCR2 = 2000; /* Output 7 */
	TIM8->CCR3 = 2000; /* Output 6 */
	TIM8->CCR4 = 2000; /* Output 5 */
}

/* *
 *
 * vSetRCOutput(channel, period [in us]):
 * Sets a new PWM match value.
 *
 * */
void vSetRCOutput(Output_Channel_Type ch, uint32_t period)
{
	/* Constant array as lookup table for the PWM channel's CCR register */
	static const uint32_t PWM_CH[8] = { (uint32_t)&TIM4->CCR4, (uint32_t)&TIM4->CCR3,
										(uint32_t)&TIM4->CCR2, (uint32_t)&TIM4->CCR1,
										(uint32_t)&TIM8->CCR4, (uint32_t)&TIM8->CCR3,
										(uint32_t)&TIM3->CCR2, (uint32_t)&TIM3->CCR1};

	if (period > RC_MAX) /* If above 2ms, set it to 2ms */
		period = RC_MAX;
	else if (period < RC_MIN) /* If below 1ms, set it to 1ms */
		period = RC_MIN;

	*((uint32_t *)PWM_CH[ch]) = period; /* Instead of big switch/if statement */
}

/* *
 *
 * vSetOutputRate(group, rate):
 * Changes the rate of the PWM between 50 and 400 Hz.
 *
 * */
void vSetOutputRate(Output_Group_Type group, PWM_Rate_Type rate)
{
	if (group == OUTPUT_1_TO_4)
		TIM4->ARR = rate;
	else if (group == OUTPUT_5_TO_6)
		TIM8->ARR = rate;
	else if (group == OUTPUT_7_TO_8)
		TIM3->ARR = rate;
}
