/* *
 *
 * Hardware Abstraction Layer for PWMs
 *
 * */

#include "pwm.h"

/* Global variable defines */

/* Private function defines */

/* Private external functions */


/* *
 * PWM Hz: 50 (5-10% duty)/400 (40-80% duty)
 *
 * Constant counter Method
 * Counter rate: 1000000 per s => Prescaler = 83
 * Period: 20000 (50Hz: 1000-2000)/2500 (400Hz: 1000-2000)
 * */

void InitPWM(void)
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
