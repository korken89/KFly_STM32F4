/* *
 *
 * Hardware Abstraction Layer for Timers
 *
 * */

#include "timer.h"
#define TIMER_RATE		1000000 /* Hz */

/* Global variable defines */

/* Private function defines */

/* Private external functions */


/* *
 *
 * Fast Counter
 * Initializes and sets up the TIM2 as a 1MHz counter.
 * Overflows every 4294 seconds.
 *
 * */
void FastCounterInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t PrescalerValue = ((SystemCoreClock /2) / TIMER_RATE) - 1;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

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
