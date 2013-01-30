/* *
 *
 * External Interrupt driver.
 * Used to check sensor data availability and RC input channels.
 *
 * */

/* Includes */
#include "exti.h"

/* Private Defines */

/* Private Typedefs */

/* Global variable defines */

/* I2C Interrupt handlers */

/* Private function defines */
static void EXTI_SensorInit(void);
static void EXTI_RCInputInit(void);

xSemaphoreHandle MPUSync, HMCSync;

void SensorInterruptInit(void)
{
	vSemaphoreCreateBinary(MPUSync);
	vSemaphoreCreateBinary(HMCSync);

	/* If an error occurs halt the execution */
	if (MPUSync == NULL)
		while(1);

	if (HMCSync == NULL)
		while(1);

	EXTI_SensorInit();
	EXTI_RCInputInit();
}

xSemaphoreHandle GetMPUSyncHandle(void)
{
	return MPUSync;
}

xSemaphoreHandle GetHMCSyncHandle(void)
{
	return HMCSync;
}

static void EXTI_SensorInit(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOG clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PG15 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect EXTI Line15 to PG15 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);

	/* Configure EXTI Line15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI15_10 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

static void EXTI_RCInputInit(void)
{

}

/* *
 * This handler executes when the data available lines on the sensors
 * fires. Line 14 is the MPU 6050 and Line 13 is the HMC5883L.
 * */
void EXTI15_10_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		/* Data available, set HMC Sync */
		xSemaphoreGiveFromISR(HMCSync, xHigherPriorityTaskWoken);

		vPortYieldFromISR(xHigherPriorityTaskWoken);

		/* Clear the EXTI line 13 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
		/* Data available, set MPU Sync */
		xSemaphoreGiveFromISR(MPUSync, xHigherPriorityTaskWoken);

		vPortYieldFromISR(xHigherPriorityTaskWoken);

		/* Clear the EXTI line 14 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}
