/* *
 *
 * External Interrupt driver.
 * Used to check sensor data availability and RC input channels.
 *
 * Semaphores are used to tell the system when there are new sensor data available.
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
static void RevMPU6050Data(uint8_t *);

xSemaphoreHandle MPU6050_DataAvailable, HMC5883L_DataAvailable;
xSemaphoreHandle I2C_Available;

void SensorsInterruptReadInit(void)
{
	vSemaphoreCreateBinary(MPU6050_DataAvailable);
	vSemaphoreCreateBinary(HMC5883L_DataAvailable);
	vSemaphoreCreateBinary(I2C_Available);

	/* If an error occurs halt the execution */
	if (MPU6050_DataAvailable == NULL)
		while(1);

	if (HMC5883L_DataAvailable == NULL)
		while(1);

	if (I2C_Available == NULL)
		while(1);

	EXTI_SensorInit();
	EXTI_RCInputInit();

	xTaskCreate(vTaskGetMPU6050Data,
			"GetMPUData",
			256,
			0,
			tskIDLE_PRIORITY + 1,
			0);
}

/* *
 *
 * Function runs if the MPU6050 interrupt has fired.
 * It will check so the I2C buss is free before starting the transfer.
 *
 * */
volatile MPU6050_Data_Union MPU6050_Data;

void vTaskGetMPU6050Data(void *pvParameters)
{
	xSemaphoreGive(I2C_Available);

	uint8_t send = MPU6050_RA_ACCEL_XOUT_H;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = MPU6050_Data.data;
	Setup.RX_Length = 14;
	Setup.Retransmissions_Max = 0;
	Setup.Callback = MPU6050ParseData;

	while(1)
	{
		xSemaphoreTake(MPU6050_DataAvailable, portMAX_DELAY);
		xSemaphoreTake(I2C_Available, portMAX_DELAY);
		I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_INTERRUPT);
	}
}

/* *
 *
 * Data has been received from the sensor and parsing can begin.
 * Notice that this is still in the interrupt.
 *
 * */
void MPU6050ParseData(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	LEDToggle(LED_RED); /* Debug LED */
	RevMPU6050Data(MPU6050_Data.data);

	xSemaphoreGiveFromISR(I2C_Available, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken != pdFALSE)
		vPortYieldFromISR();
}

static void EXTI_SensorInit(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC14 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect EXTI Line15 to PC14 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);

	/* Configure EXTI Line14 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

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
		xSemaphoreGiveFromISR(HMC5883L_DataAvailable, &xHigherPriorityTaskWoken);

		if (xHigherPriorityTaskWoken != pdFALSE)
			vPortYieldFromISR();

		/* Clear the EXTI line 13 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
		/* Data available, set MPU Sync */
		xSemaphoreGiveFromISR(MPU6050_DataAvailable, &xHigherPriorityTaskWoken);

		if (xHigherPriorityTaskWoken != pdFALSE)
			vPortYieldFromISR();

		/* Clear the EXTI line 14 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}

static void RevMPU6050Data(uint8_t *data)
{
	uint8_t temp;

	for (int i = 0; i < 14; i += 2)
	{
		temp = data[i];
		data[i] = data[i+1];
		data[i+1] = temp;
	}
}
