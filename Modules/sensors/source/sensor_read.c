/* *
 *
 * Sensor data read out driver.
 * Used to check sensor data availability.
 *
 * Semaphores are used to tell the system when there are new sensor data available.
 *
 * */

/* Includes */
#include "sensor_read.h"

/* Private Defines */

/* Private Typedefs */

/* Private variable defines */
static Sensor_Data_Type Sensor_Data;			/* Holds the sensor data after applying calibration */
static Sensor_Raw_Data_Type Sensor_Raw_Data;	/* Holds the raw sensor data */

/* Global variable defines */

/* Private function defines */
static void MPU6050ParseData(void);
static void HMC5983ParseData(void);
static void EXTI_SensorInit(void);
static void EXTI_MPU6050Init(void);
static void EXTI_HMC5983Init(void);
static void RevMPU6050Data(uint8_t *);
static void RevHMC5983Data(uint8_t *);

xTaskHandle MPU6050Handle, HMC5983Handle;
xSemaphoreHandle I2C_Available;
xSemaphoreHandle NewMeasurementAvaiable;
Sensor_Calibration_Type *sensor_calibration;

/* Conversion union for sensor data */
static MPU6050_Data_Union MPU6050_Data;
static HMC5983_Data_Union HMC5983_Data;
static float MPU6050_Gyro_Gain;

void SensorsInterruptReadInit(void)
{
	vSemaphoreCreateBinary(I2C_Available);
	vSemaphoreCreateBinary(NewMeasurementAvaiable);

	/* If an error occurs halt the execution */
	if (I2C_Available == NULL)
		while(1);
	if (NewMeasurementAvaiable == NULL)
		while(1);

	xSemaphoreGive(I2C_Available);

	EXTI_SensorInit();

	sensor_calibration = ptrGetSensorCalibration();
	MPU6050_Gyro_Gain = GetMPU6050GyroGain();
	vInitSensorCalibration(sensor_calibration);

	/* Zero Sensor_Data */
	Sensor_Data.acc.x = 0.0f;
	Sensor_Data.acc.y = 0.0f;
	Sensor_Data.acc.z = 0.0f;
	Sensor_Data.gyro.x = 0.0f;
	Sensor_Data.gyro.y = 0.0f;
	Sensor_Data.gyro.z = 0.0f;
	Sensor_Data.mag.x = 0.0f;
	Sensor_Data.mag.y = 0.0f;
	Sensor_Data.mag.z = 0.0f;
	Sensor_Data.pressure = 0;

	/* Zero raw sensor data */
	Sensor_Raw_Data.acc_x = 0;
	Sensor_Raw_Data.acc_y = 0;
	Sensor_Raw_Data.acc_z = 0;
	Sensor_Raw_Data.gyro_x = 0;
	Sensor_Raw_Data.gyro_y = 0;
	Sensor_Raw_Data.gyro_z = 0;
	Sensor_Raw_Data.mag_x = 0;
	Sensor_Raw_Data.mag_y = 0;
	Sensor_Raw_Data.mag_z = 0;
	Sensor_Raw_Data.pressure = 0;

	/* Create task to retrieve Accelerometer and Gyro data */
	xTaskCreate(vTaskGetMPU6050Data,
			"Get MPU6050 Data",
			configMINIMAL_STACK_SIZE,
			0,
			configMAX_PRIORITIES - 1, // Highest priority available
			&MPU6050Handle);

	/* Create task to retrieve Magnetometer data */
	xTaskCreate(vTaskGetHMC5983Data,
			"Get HMC5983 Data",
			configMINIMAL_STACK_SIZE,
			0,
			configMAX_PRIORITIES - 1, // Highest priority available
			&HMC5983Handle);
}

Sensor_Data_Type *ptrGetSensorDataPointer(void)
{
	return &Sensor_Data;
}

Sensor_Raw_Data_Type *ptrGetRawSensorDataPointer(void)
{
	return &Sensor_Raw_Data;
}

/* *
 *
 * Function runs if the MPU6050 interrupt has fired.
 * It will check so the I2C buss is free before starting the transfer.
 *
 * */
void vTaskGetMPU6050Data(void *pvParameters)
{
	uint8_t send = MPU6050_RA_ACCEL_XOUT_H;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = MPU6050_Data.data;
	Setup.RX_Length = 14;
	Setup.Retransmissions_Max = 5;
	Setup.Callback = MPU6050ParseData;

	while(1)
	{
		vTaskSuspend(NULL);								/* Suspend until there is data available */
		xSemaphoreTake(I2C_Available, portMAX_DELAY);	/* Wait until I2C bus is free, then claim it */
		I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_INTERRUPT);
	}
}

/* *
 *
 * Data has been received from the sensor and parsing can begin.
 * Notice that this is still in the interrupt.
 *
 * */

static void MPU6050ParseData(void)
{
	Bool yield = FALSE;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Release the I2C */
	xSemaphoreGiveFromISR(I2C_Available, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken != pdFALSE)
		yield = TRUE;

	xHigherPriorityTaskWoken = pdFALSE;

	RevMPU6050Data(MPU6050_Data.data);

	/* Move the raw data to the raw data structure */
	Sensor_Raw_Data.acc_x =   MPU6050_Data.value.acc_y;
	Sensor_Raw_Data.acc_y = - MPU6050_Data.value.acc_x;
	Sensor_Raw_Data.acc_z =   MPU6050_Data.value.acc_z;

	Sensor_Raw_Data.gyro_x =   MPU6050_Data.value.gyro_y;
	Sensor_Raw_Data.gyro_y = - MPU6050_Data.value.gyro_x;
	Sensor_Raw_Data.gyro_z =   MPU6050_Data.value.gyro_z;

	/* Move the data to the public data holder and compensate for gains and biases from calibration */
	Sensor_Data.acc.x = (((float)Sensor_Raw_Data.acc_x) - sensor_calibration->accelerometer_bias.x) / sensor_calibration->accelerometer_gain.x;
	Sensor_Data.acc.y = (((float)Sensor_Raw_Data.acc_y) - sensor_calibration->accelerometer_bias.y) / sensor_calibration->accelerometer_gain.y;
	Sensor_Data.acc.z = (((float)Sensor_Raw_Data.acc_z) - sensor_calibration->accelerometer_bias.z) / sensor_calibration->accelerometer_gain.z;

	Sensor_Data.gyro.x = ((float)Sensor_Raw_Data.gyro_x) * MPU6050_Gyro_Gain;
	Sensor_Data.gyro.y = ((float)Sensor_Raw_Data.gyro_y) * MPU6050_Gyro_Gain;
	Sensor_Data.gyro.z = ((float)Sensor_Raw_Data.gyro_z) * MPU6050_Gyro_Gain;

	/* New measurement data available */
	xSemaphoreGiveFromISR(NewMeasurementAvaiable, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken != pdFALSE)
		yield = TRUE;
	
	if (yield == TRUE)
		vPortYieldFromISR();
}

/* *
 *
 * Function runs if the MPU6050 interrupt has fired.
 * It will check so the I2C buss is free before starting the transfer.
 *
 * */
void vTaskGetHMC5983Data(void *pvParameters)
{
	uint8_t send = HMC5983_RA_DATAX_H;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = HMC5983_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = HMC5983_Data.data;
	Setup.RX_Length = 6;
	Setup.Retransmissions_Max = 5;
	Setup.Callback = HMC5983ParseData;

	while(1)
	{
		vTaskSuspend(NULL);								/* Suspend until there is data available */
		xSemaphoreTake(I2C_Available, portMAX_DELAY);	/* Wait until I2C bus is free, then claim it */
		I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_INTERRUPT);
	}
}

/* *
 *
 * Data has been received from the sensor and parsing can begin.
 * Notice that this is still in the interrupt.
 *
 * */
static void HMC5983ParseData(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Release the I2C */
	xSemaphoreGiveFromISR(I2C_Available, &xHigherPriorityTaskWoken);

	RevHMC5983Data(HMC5983_Data.data);

	/* Move the raw data to the raw data structure */
	Sensor_Raw_Data.mag_x = - HMC5983_Data.value.mag_y;
	Sensor_Raw_Data.mag_y =   HMC5983_Data.value.mag_x;
	Sensor_Raw_Data.mag_z = - HMC5983_Data.value.mag_z;

	/* Move the data to the public data holder and convert signs */
	Sensor_Data.mag.x = (((float)Sensor_Raw_Data.mag_x) - sensor_calibration->magnetometer_bias.x) / sensor_calibration->magnetometer_gain.x;
	Sensor_Data.mag.y = (((float)Sensor_Raw_Data.mag_y) - sensor_calibration->magnetometer_bias.y) / sensor_calibration->magnetometer_gain.y;
	Sensor_Data.mag.z = (((float)Sensor_Raw_Data.mag_z) - sensor_calibration->magnetometer_bias.z) / sensor_calibration->magnetometer_gain.z;

	if (xHigherPriorityTaskWoken != pdFALSE)
		vPortYieldFromISR();
}

static void EXTI_SensorInit(void)
{
	EXTI_MPU6050Init();
	EXTI_HMC5983Init();
}

static void EXTI_MPU6050Init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC14 pin as input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect EXTI Line14 to PC14 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);

	/* Configure EXTI Line14 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

static void EXTI_HMC5983Init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC13 pin as input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect EXTI Line13 to PC13 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	/* Configure EXTI Line13 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

/* *
 * This handler executes when the data ready from the sensors
 * fires. Line 14 is the MPU 6050 and Line 13 is the HMC5983.
 * */
void EXTI15_10_IRQHandler(void)
{
	Bool yield = FALSE;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		/* Clear the EXTI line 13 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line13);

		/* Data available, resume HMC5983 data receiving task */
		xHigherPriorityTaskWoken = xTaskResumeFromISR(HMC5983Handle);

		if (xHigherPriorityTaskWoken != pdFALSE)
			yield = TRUE;
	}

	xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
		/* Clear the EXTI line 14 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line14);

		/* Data available, resume MPU6050 data receiving task */
		xHigherPriorityTaskWoken = xTaskResumeFromISR(MPU6050Handle);

		if (xHigherPriorityTaskWoken != pdFALSE)
			yield = TRUE;
	}

	if (yield == TRUE)
			vPortYieldFromISR();
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

static void RevHMC5983Data(uint8_t *data)
{
	uint8_t temp;

	for (int i = 0; i < 6; i += 2)
	{
		temp = data[i];
		data[i] = data[i+1];
		data[i+1] = temp;
	}
}
