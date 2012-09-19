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
 * PWM Hz: 50 (5-10% duty) / 400 (40-80% duty)
 *
 * Constant counter Method
 * Counter rate: 1000000 per s => Prescaler = 83
 * Period: 20000 (50Hz: 1000-2000 CCR) / 2500 (400Hz: 1000-2000 CCR)
 * */

void InitPWM(void)
{
	/* TODO: Add PWM setup  */

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM config */
	GPIO_InitTypeDef GPIO_InitStructure;

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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
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



}
