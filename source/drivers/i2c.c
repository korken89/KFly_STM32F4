/* *
 *
 * Hardware Communication Layer for the I2C bus
 *
 * TODO:
 * Everything...
 *
 * */


#include "i2c.h"

/* Private Defines */
#define I2CBus                    I2C2
#define RCC_APB1Periph_I2Cx       RCC_APB1Periph_I2C2
#define RCC_AHB1Periph_GPIO_SCL   RCC_AHB1Periph_GPIOB
#define RCC_AHB1Periph_GPIO_SDA   RCC_AHB1Periph_GPIOB
#define GPIO_AF_I2Cx              GPIO_AF_I2C2
#define GPIO_SCL                  GPIOB
#define GPIO_SDA                  GPIOB
#define GPIO_Pin_SCL              GPIO_Pin_10
#define GPIO_Pin_SDA              GPIO_Pin_11
#define GPIO_PinSource_SCL        GPIO_PinSource10
#define GPIO_PinSource_SDA        GPIO_PinSource11

/* Gobal variable defines */
volatile I2C_MASTER_SETUP_Type *I2Ctmp[3]; /* Pointer to I2C Config Setup */

/* Private Typedefs */

/* *
 * The I2C read on the STM32F4xx is not symetric so special care
 * must be taken if the number of bytes to be read are one, two
 * or more than two.
 *
 * This enum identifies, for the interrupt handeler, which size
 * of read that shall be performed.
 * */
typedef enum
{
	I2C_READ_ONE = 0,
	I2C_READ_TWO,
	I2C_READ_MORE_THAN_TWO
} I2C_READ_LENGTH_Type;

/* Private function defines */
int8_t I2C_getNum(I2C_TypeDef *);
void I2C2_EV_IRQHandler(void); /* These two functions shall be moved to stm32f4xx_it.c */
void I2C2_ER_IRQHandler(void);
static uint16_t I2C_Addr(I2C_TypeDef *, uint8_t DevAddr, uint8_t dir);
static uint16_t I2C_SendByte(I2C_TypeDef *, uint8_t);
static uint16_t I2C_Start(I2C_TypeDef *);
static uint16_t WaitSR1FlagsSet(I2C_TypeDef *, uint16_t);
static uint32_t WaitLineIdle(I2C_TypeDef *);

/* Private external functions */

void InitSensorBus(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

	// Enable the i2c
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE);
	// Reset the Peripheral
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, DISABLE);

	// Enable the GPIOs for the SCL/SDA Pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIO_SCL | RCC_AHB1Periph_GPIO_SDA, ENABLE);

	// Configure and initialize the GPIOs
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIO_SCL, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_SDA;
	GPIO_Init(GPIO_SDA, &GPIO_InitStructure);

	// Connect GPIO pins to peripheral
	GPIO_PinAFConfig(GPIO_SCL, GPIO_PinSource_SCL, GPIO_AF_I2Cx);
	GPIO_PinAFConfig(GPIO_SDA, GPIO_PinSource_SDA, GPIO_AF_I2Cx);

	// Configure and Initialize the I2C
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;

	// Initialize the Peripheral
	I2C_Init(I2CBus, &I2C_InitStructure);

	// Enable interrupts
	I2C_ITConfig(I2CBus, (I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR), ENABLE);

	// I2C Peripheral Enable
	I2C_Cmd(I2CBus, ENABLE);
}

int8_t I2C_getNum(I2C_TypeDef *I2Cx)
{
	if (I2Cx == I2C1)
		return 0;

	else if (I2Cx == I2C2)
		return 1;

	else if (I2Cx == I2C3)
		return 2;

	else
		return -1;
}

ErrorStatus I2C_MasterTransferData(I2C_TypeDef *I2Cx, I2C_MASTER_SETUP_Type *TransferCfg, I2C_TRANSFER_OPTION_Type Opt)
{
	uint8_t *txdat;
	uint8_t *rxdat;
	uint8_t tmp;

	if (Opt == I2C_TRANSFER_POLLING)
	{
		/* First Start condition */
		TransferCfg->Retransmissions_Count = 0;

		// Reset all default state
		txdat = TransferCfg->TX_Data;
		rxdat = TransferCfg->RX_Data;

		// Reset I2C setup value to default state
		TransferCfg->TX_Count = 0;
		TransferCfg->RX_Count = 0;
		TransferCfg->Status = 0;

		// Start command
		TransferCfg->Status = I2C_Start(I2Cx);

		/* *
		 * TODO:
		 * Check for errors
		 * */

		/* In case of sending data first */
		if ((TransferCfg->TX_Length != 0) && (TransferCfg->TX_Data != NULL))
		{
			/* Send slave address + WR direction bit = 0 */
			TransferCfg->Status = I2C_Addr(I2Cx, TransferCfg->Slave_Address_7bit, 0);
			(void) I2Cx->SR2;

			/* *
			 * TODO:
			 * Check for errors.
			 * */

			/* Send a number of data bytes */
			while (TransferCfg->TX_Count < TransferCfg->TX_Length)
			{
				/* *
				 * TODO:
				 * Check for errors.
				 * */
				I2C_SendByte(I2Cx, *txdat);

				txdat++;
				TransferCfg->TX_Count++;
			}

			WaitSR1FlagsSet(I2Cx, I2C_SR1_BTF);
		}

		/* Second Start condition (Repeat Start) */
		if ((TransferCfg->TX_Length != 0) && (TransferCfg->TX_Data != NULL) && (TransferCfg->RX_Length != 0) && (TransferCfg->RX_Data != NULL))
		{
			TransferCfg->Status = I2C_Start(I2Cx);
			(void) I2Cx->SR2;

			/* *
			 * TODO:
			 * Check for errors.
			 * */
		}

		/* Then, start reading after sending data */
		if ((TransferCfg->RX_Length != 0) && (TransferCfg->RX_Data != NULL))
		{
			/* Send slave address + RD direction bit = 1 */
			TransferCfg->Status = I2C_Addr(I2Cx, TransferCfg->Slave_Address_7bit, 1);

			/* *
			 * TODO:
			 * Check for errors.
			 * */

			/* Receive a number of data bytes */
			while (TransferCfg->RX_Count < TransferCfg->RX_Length)
			{
				/* *
				 * TODO:
				 * Add intellegence for the nonsymetry of the I2C.
				 * */
			}
		}

		/* *
		 * TODO:
		 * When transfer is completed, send stop
		 * */
		WaitLineIdle(I2Cx);
	}
	else
	{
	}

	return SUCCESS;
}

void I2C_MasterHandler(I2C_TypeDef *I2Cx)
{
	int8_t I2C_num = I2C_getNum(I2Cx);
	uint16_t status = I2Cx->SR1 & I2C_SR1_BITMASK;
	I2C_MASTER_SETUP_Type *RXTX_Setup = I2Ctmp[I2C_num];

	switch (status & I2C_STATUS_BITMASK)
	{
		case I2C_STATUS_SB:

			break;

		case I2C_STATUS_ADDR:

			break;

		case I2C_STATUS_BTF:

			break;

		case I2C_STATUS_RNE:

			break;

		case I2C_STATUS_TE:

			break;

		default:
			break;
	}
}

void I2C2_EV_IRQHandler(void)
{
	I2C_MasterHandler(I2C2);
}

void I2C2_ER_IRQHandler(void)
{
	I2C_MasterHandler(I2C2);
}

static uint16_t I2C_Addr(I2C_TypeDef *I2Cx, uint8_t DevAddr, uint8_t dir)
{
	//Write address to the DR (to the bus)
	I2Cx->DR = (DevAddr << 1) | dir;

	//Wait till ADDR is set (ADDR is set when the slave sends ACK to the address).
	//Clock streches till ADDR is Reset. To reset the hardware i)Read the SR1 ii)Wait till ADDR is Set iii)Read SR2
	//Note1: Spec_p602 recommends the waiting operation
	//Note2: We don't read SR2 here. Therefore the clock is going to be streched even after return from this function
	return WaitSR1FlagsSet(I2Cx, I2C_SR1_ADDR);
}

static uint16_t I2C_SendByte(I2C_TypeDef *I2Cx, uint8_t data)
{
	//Write address to the DR (to the bus)
	I2Cx->DR = data;

	//Wait till ADDR is set (ADDR is set when the slave sends ACK to the address).
	//Clock streches till ADDR is Reset. To reset the hardware i)Read the SR1 ii)Wait till ADDR is Set iii)Read SR2
	//Note1: Spec_p602 recommends the waiting operation
	//Note2: We don't read SR2 here. Therefore the clock is going to be streched even after return from this function
	return WaitSR1FlagsSet(I2Cx, I2C_SR1_ADDR);
}

static uint16_t I2C_Start(I2C_TypeDef *I2Cx)
{
	//Generate a start condition. (As soon as the line becomes idle, a Start condition will be generated)
	I2Cx->CR1 |= I2C_CR1_START;

	//When start condition is generated SB is set and clock is stretched.
	//To activate the clock again i)read SR1 ii)write something to DR (e.g. address)
	return WaitSR1FlagsSet(I2Cx, I2C_SR1_SB);  //Wait till SB is set
}

static uint16_t WaitSR1FlagsSet(I2C_TypeDef *I2Cx, uint16_t Flags)
{
	//Wait till the specified SR1 Bits are set
	//More than 1 Flag can be "or"ed. This routine reads only SR1.
	uint32_t TimeOut = HSI_VALUE;

	while(((I2Cx->SR1) & Flags) != Flags)
		if (!(TimeOut--))
			return 1;

	return 0;
}


static uint32_t WaitLineIdle(I2C_TypeDef *I2Cx)
{
	//Wait till the Line becomes idle.

	uint32_t TimeOut = HSI_VALUE;
	//Check to see if the Line is busy
	//This bit is set automatically when a start condition is broadcasted on the line (even from another master)
	//and is reset when stop condition is detected.
	while((I2Cx->SR2) & (I2C_SR2_BUSY))
		if (!(TimeOut--))
			return 1;

  return 0;
}
