/* *
 *
 * Hardware Abstraction Layer for Timers
 *
 * */

#include "timer.h"

/* Gobal variable defines */

/* Private function defines */

/* Private external functions */


void InitFastCounter(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t PrescalerValue = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    PrescalerValue = ((SystemCoreClock /2) / 1000000) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM2, ENABLE);
}

uint32_t GetFastCounterValue(void)
{
	return TIM_GetCounter(TIM2);
}
