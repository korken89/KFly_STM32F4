/* *
 *
 * Hardware Communication Layer for the I2C bus
 *
 * */


#include "i2c.h"

/* Private Defines */
#define I2CBus                    	I2C2
#define RCC_APB1Periph_I2Cx       	RCC_APB1Periph_I2C2
#define RCC_AHB1Periph_GPIO_SCL   	RCC_AHB1Periph_GPIOB
#define RCC_AHB1Periph_GPIO_SDA   	RCC_AHB1Periph_GPIOB
#define GPIO_AF_I2Cx              	GPIO_AF_I2C2
#define GPIO_SCL                  	GPIOB
#define GPIO_SDA                  	GPIOB
#define GPIO_Pin_SCL              	GPIO_Pin_10
#define GPIO_Pin_SDA              	GPIO_Pin_11
#define GPIO_PinSource_SCL        	GPIO_PinSource10
#define GPIO_PinSource_SDA        	GPIO_PinSource11

/* Private Typedefs */
typedef struct
{
	I2C_MASTER_SETUP_Type *RXTX_Setup;		/* Transmission setup */
	I2C_DIRECTION_Type Direction;			/* Current direction phase */
	void (*inthandler)(I2C_TypeDef *I2Cx);	/* Transmission interrupt handler */
} I2C_INT_CFG_Type;

/* Gobal variable defines */
volatile I2C_INT_CFG_Type I2Ctmp[3]; /* Pointer to I2C Config Setup */


/* Private function defines */
int8_t I2C_getNum(I2C_TypeDef *);

void I2C2_EV_IRQHandler(void); /* These two functions shall be moved to stm32f4xx_it.c */
void I2C2_ER_IRQHandler(void);
void I2C_MasterHandler(I2C_TypeDef *);

static uint16_t I2C_Addr(I2C_TypeDef *, uint8_t, uint8_t);
static uint16_t I2C_SendByte(I2C_TypeDef *, uint8_t);
static uint16_t I2C_Read(I2C_TypeDef *, uint8_t *);
static uint16_t I2C_Start(I2C_TypeDef *);
static uint16_t WaitSR1FlagsSet(I2C_TypeDef *, uint16_t);
static uint16_t WaitLineIdle(I2C_TypeDef *);

/* Private external functions */

void InitSensorBus(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

	/* Enable the i2c */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE);
	/* Reset the Peripheral */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2Cx, DISABLE);

	/* Enable the GPIOs for the SCL/SDA Pins */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIO_SCL | RCC_AHB1Periph_GPIO_SDA, ENABLE);

	/* Configure and initialize the GPIOs */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIO_SCL, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_SDA;
	GPIO_Init(GPIO_SDA, &GPIO_InitStructure);

	/* Connect GPIO pins to peripheral */
	GPIO_PinAFConfig(GPIO_SCL, GPIO_PinSource_SCL, GPIO_AF_I2Cx);
	GPIO_PinAFConfig(GPIO_SDA, GPIO_PinSource_SDA, GPIO_AF_I2Cx);

	/* Configure and Initialize the I2C */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 50000;

	/* Initialize the Peripheral */
	I2C_Init(I2CBus, &I2C_InitStructure);

	/* I2C Peripheral Enable */
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
		TransferCfg->Retransmissions_Count = 0;
retry:
		I2Cx->SR1 &= ~(I2Cx->SR1 & I2C_ERROR_BITMASK); /* Clear errors */
		if (TransferCfg->Retransmissions_Count > TransferCfg->Retransmissions_Max)
		{ /* Maximum number of retransmissions reached, abort */
			I2Cx->CR1 |= I2C_CR1_STOP;
			return ERROR;
		}
		/* Reset all values to default state */
		txdat = TransferCfg->TX_Data;
		rxdat = TransferCfg->RX_Data;

		/* Reset I2C setup value to default state */
		TransferCfg->TX_Count = 0;
		TransferCfg->RX_Count = 0;
		TransferCfg->Status = 0;

		/* Send start */
		TransferCfg->Status = I2C_Start(I2Cx);
		if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
		{
			TransferCfg->Retransmissions_Count++;
			goto retry;
		}

		/* In case of sending data first */
		if ((TransferCfg->TX_Length != 0) && (TransferCfg->TX_Data != NULL))
		{
			/* Send slave address + WR direction bit = 0 */
			TransferCfg->Status = I2C_Addr(I2Cx, TransferCfg->Slave_Address_7bit, 0);
			if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
			{
				TransferCfg->Retransmissions_Count++;
				goto retry;
			}
			(void)I2Cx->SR2;

			/* Send a number of data bytes */
			while (TransferCfg->TX_Count < TransferCfg->TX_Length)
			{
				TransferCfg->Status = I2C_SendByte(I2Cx, *(txdat++));
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->TX_Count++;
			}

			TransferCfg->Status = WaitSR1FlagsSet(I2Cx, I2C_SR1_BTF);
			if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
			{
				TransferCfg->Retransmissions_Count++;
				goto retry;
			}

			/* Send Stop if no data is to be received */
			if ((TransferCfg->RX_Length == 0) || (TransferCfg->RX_Data == NULL))
			{
				I2Cx->CR1 |= I2C_CR1_STOP;
				TransferCfg->Status = WaitLineIdle(I2Cx);
				return SUCCESS;
			}
		}

		/* Second Start condition (Repeat Start) */
		if ((TransferCfg->TX_Length != 0) && (TransferCfg->TX_Data != NULL) && \
			(TransferCfg->RX_Length != 0) && (TransferCfg->RX_Data != NULL))
		{
			TransferCfg->Status = I2C_Start(I2Cx);
			if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
			{
				TransferCfg->Retransmissions_Count++;
				goto retry;
			}
			(void)I2Cx->SR2;
		}

		/* *
		 *
		 * --------------------------- RECIEVE PHASE ---------------------------
		 * The I2C read on the STM32F4xx is not symetric so special
		 * care must be taken if the number of bytes to be read are
		 * one, two or more than two.
		 *
		 * */


		/* Then, start reading after sending data */
		if ((TransferCfg->RX_Length != 0) && (TransferCfg->RX_Data != NULL))
		{
			/* Send slave address + RD direction bit = 1 */
			TransferCfg->Status = I2C_Addr(I2Cx, TransferCfg->Slave_Address_7bit, 1);
			if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
			{
				TransferCfg->Retransmissions_Count++;
				goto retry;
			}

			/* Receive a number of data bytes */

			if (TransferCfg->RX_Length == 1) /* We are going to read only 1 byte */
			{
				/* Before Clearing Addr bit by reading SR2, we have to cancel ack. */
				I2Cx->CR1 &= ~I2C_CR1_ACK;

				/* Now Read the SR2 to clear ADDR */
				(void)I2Cx->SR2;

				/* Order a STOP condition */
				/* Note: Spec_p583 says this should be done just after clearing ADDR */
				/* If it is done before ADDR is set, a STOP is generated immediately as the clock is being streched */
				I2Cx->CR1 |= I2C_CR1_STOP;
				/* Be carefull that till the stop condition is actually transmitted the clock will stay active
				 * even if a NACK is generated after the next received byte. */

				/* Read the next byte */
				TransferCfg->Status = I2C_Read(I2Cx, rxdat);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->RX_Count++;

				/* Make Sure Stop bit is cleared and Line is now Iddle */
				TransferCfg->Status = WaitLineIdle(I2Cx);

				/* Enable the Acknowledgement again */
				I2Cx->CR1 |= ((uint16_t)I2C_CR1_ACK);
			}

			else if (TransferCfg->RX_Length == 2) /* We are going to read 2 bytes (See: Spec_p584) */
			{
				/* Before Clearing Addr, reset ACK, set POS */
				I2Cx->CR1 &= ~I2C_CR1_ACK;
				I2Cx->CR1 |= I2C_CR1_POS;

				/* Read the SR2 to clear ADDR */
				(void)I2Cx->SR2;

				/* Wait for the next 2 bytes to be received (1st in the DR, 2nd in the shift register) */
				TransferCfg->Status = WaitSR1FlagsSet(I2Cx, I2C_SR1_BTF);
				/* As we don't read anything from the DR, the clock is now being strecthed. */

				/* Order a stop condition (as the clock is being strecthed, the stop condition is generated immediately) */
				I2Cx->CR1 |= I2C_CR1_STOP;

				/* Read the next two bytes */
				TransferCfg->Status = I2C_Read(I2Cx, rxdat++);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->RX_Count++;

				TransferCfg->Status = I2C_Read(I2Cx, rxdat);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->RX_Count++;

				/* Make Sure Stop bit is cleared and Line is now Idle */
				TransferCfg->Status = WaitLineIdle(I2Cx);

				/* Enable the ack and reset Pos */
				I2Cx->CR1 |= I2C_CR1_ACK;
				I2Cx->CR1 &= ~I2C_CR1_POS;
			}
			else /* We have more than 2 bytes. See spec_p585 */
			{
				/* Read the SR2 to clear ADDR */
				(void)I2Cx->SR2;

				while((TransferCfg->RX_Length - TransferCfg->RX_Count) > 3) /* Read till the last 3 bytes */
				{
					TransferCfg->Status = I2C_Read(I2Cx, rxdat++);
					if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
					{
						TransferCfg->Retransmissions_Count++;
						goto retry;
					}
					TransferCfg->RX_Count++;
				}

				/* 3 more bytes to read. Wait till the next to is actually received */
				TransferCfg->Status = WaitSR1FlagsSet(I2Cx, I2C_SR1_BTF);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				/* Here the clock is strecthed. One more to read. */

				/* Reset Ack */
				I2Cx->CR1 &= ~I2C_CR1_ACK;

				/* Read N-2 */
				TransferCfg->Status = I2C_Read(I2Cx, rxdat++);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->RX_Count++;

				/* Once we read this, N is going to be read to the shift register and NACK is generated */

				/* Wait for the BTF */
				TransferCfg->Status = WaitSR1FlagsSet(I2Cx, I2C_SR1_BTF); /* N-1 is in DR, N is in shift register */
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				/* Here the clock is stretched */

				/* Generate a stop condition */
				I2Cx->CR1 |= I2C_CR1_STOP;

				/* Read the last two bytes (N-1 and N) */
				TransferCfg->Status = I2C_Read(I2Cx, rxdat++);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->RX_Count++;

				TransferCfg->Status = I2C_Read(I2Cx, rxdat);
				if (TransferCfg->Status & (I2C_ERROR_BIT | I2C_ERROR_BITMASK)) /* Check for errors */
				{
					TransferCfg->Retransmissions_Count++;
					goto retry;
				}
				TransferCfg->RX_Count++;

				/* Make Sure Stop bit is cleared and Line is now Idle  */
				TransferCfg->Status = WaitLineIdle(I2Cx);

				/* Enable the ACK */
				I2Cx->CR1 |= I2C_CR1_ACK;
			}
		}
	}
	else /* Interrupt transfer */
	{
		/* *
		 * Add initialization
		 * */

		TransferCfg->Status = 0;
		TransferCfg->TX_Count = 0;
		TransferCfg->RX_Count = 0;
		TransferCfg->Retransmissions_Count = 0;

		I2Ctmp[I2C_getNum(I2Cx)].Direction = I2C_SENDING;
		I2Ctmp[I2C_getNum(I2Cx)].RXTX_Setup = TransferCfg;
		I2Ctmp[I2C_getNum(I2Cx)].inthandler = I2C_MasterHandler;

		/*  Enable interrupts */
		I2C_ITConfig(I2Cx, (I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR), ENABLE);
		I2Cx->CR1 |= I2C_CR1_START; /* Send start condition to start the transfer */
	}

	return SUCCESS;
}

void I2C_MasterHandler(I2C_TypeDef *I2Cx)
{
	int8_t I2C_num = I2C_getNum(I2Cx);
	uint16_t status = I2Cx->SR1 & I2C_SR1_BITMASK;
	I2C_MASTER_SETUP_Type *TransferCfg = I2Ctmp[I2C_num].RXTX_Setup;

	/* *
	 *
	 * Note to self:
	 * In order to prevent repeated running of the ISR, when TXE or RXNE = 1 and
	 * awaiting BTF = 1, turn off the buffer interrupts (ITBUFEN) and just await
	 * the BTF interrupt. This should break the "infinite ISR recall"-loop.
	 *
	 * */


	if (TransferCfg->Retransmissions_Count > TransferCfg->Retransmissions_Max)
	{ /* Maximum number of retransmissions reached, abort */
		I2Cx->CR1 |= I2C_CR1_STOP;

		/* *
		 * TODO:
		 * Disable interrupts
		 * */
	}

	if (status & I2C_ERROR_BITMASK) /* Error */
	{
		I2Cx->SR1 &= ~(I2Cx->SR1 & I2C_ERROR_BITMASK); /* Clear errors */
		TransferCfg->Retransmissions_Count++;

		/* *
		 * TODO:
		 * Generate new start
		 * Reset all variables
		 * */
	}

	if (I2Ctmp[I2C_num].Direction == I2C_SENDING) /* Sending data */
	{
		switch (status & I2C_STATUS_BITMASK)
		{
			case I2C_SR1_SB: /* Start condition sent */
				break;

			case I2C_SR1_ADDR: /* Address+W sent, ack receieved */
				break;

			case I2C_SR1_TXE: /* Data has been sent to the shift register, transmit register empty */
				break;

			case (I2C_SR1_TXE | I2C_SR1_BTF): /* Shift register and transmit register empty */
				break;

			default:
				break;
		}
	}

	/* *
	 *
	 * ----------------------------- RECIEVE PHASE -----------------------------
	 * The I2C read on the STM32F4xx is not symetric so special
	 * care must be taken if the number of bytes to be read are
	 * one, two or more than two.
	 *
	 * Note to self:
	 * I will design the receiving part as the I2C_RdBufEasy in yigiter's example.
	 * However some extra logic will be added to handle the special case of only
	 * one byte being received. Hopefully I won't have to design for all the
	 * special cases as I did in the polled read. By doing this the the BTF flag
	 * shall never be set.
	 *
	 * */

	else
	{
		switch (status & (I2C_STATUS_BITMASK & ~I2C_SR1_BTF)) /* Just in case remove the BTF flag */
		{
			case I2C_SR1_SB: /* Start/Restart condition sent */
				break;

			case I2C_SR1_ADDR: /* Address+R sent, ack receieved */
				break;

			case I2C_SR1_RXNE: /* Data has been sent to the data register, ready for read out */
				break;

			default:
				break;
		}
	}
}

void I2C2_EV_IRQHandler(void)
{
	int8_t I2C_num = I2C_getNum(I2C2);
	I2Ctmp[I2C_num].inthandler(I2C2);
}

void I2C2_ER_IRQHandler(void)
{
	int8_t I2C_num = I2C_getNum(I2C2);
	I2Ctmp[I2C_num].inthandler(I2C2);
}

static uint16_t I2C_Addr(I2C_TypeDef *I2Cx, uint8_t DevAddr, uint8_t dir)
{
	/*  Write address to the DR */
	I2Cx->DR = (DevAddr << 1) | (dir & 0x01); /* Or in the lowest bit in dir */

	/* Wait till ADDR is set (ADDR is set when the slave sends ACK to the address). */
	/* Clock streches till ADDR is Reset. To reset the hardware i)Read the SR1 ii)Wait till ADDR is Set iii)Read SR2 */
	/* Note1: Spec_p602 recommends the waiting operation */
	/* Note2: We don't read SR2 here. Therefore the clock is going to be streched even after return from this function */
	return WaitSR1FlagsSet(I2Cx, I2C_SR1_ADDR);
}

static uint16_t I2C_SendByte(I2C_TypeDef *I2Cx, uint8_t data)
{
	/* Write data to the DR */
	I2Cx->DR = data;

	return WaitSR1FlagsSet(I2Cx, I2C_SR1_TXE);
}

static uint16_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t *pBuf)
{
	uint32_t err;
    uint32_t TimeOut = HSI_VALUE;

	while(((I2Cx->SR1) & I2C_SR1_RXNE) != I2C_SR1_RXNE)
		if (!(TimeOut--))
			return ((I2Cx->SR1 & I2C_SR1_BITMASK) | I2C_ERROR_BIT);

	*pBuf = I2Cx->DR;   /*This clears the RXNE bit. IF both RXNE and BTF is set, the clock stretches */
	return (I2Cx->SR1 & I2C_SR1_BITMASK);

}

static uint16_t I2C_Start(I2C_TypeDef *I2Cx)
{
	/* Generate a start condition. (As soon as the line becomes idle, a Start condition will be generated) */
	I2Cx->CR1 |= I2C_CR1_START;

	/* When start condition is generated SB is set and clock is stretched. */
	/* To activate the clock again i)read SR1 ii)write something to DR (e.g. address) */
	return WaitSR1FlagsSet(I2Cx, I2C_SR1_SB);  /* Wait till SB is set */
}

static uint16_t WaitSR1FlagsSet(I2C_TypeDef *I2Cx, uint16_t Flags)
{
	/* Wait till the specified SR1 Bits are set */
	/* More than 1 Flag can be "or"ed. This routine reads only SR1. */
	uint32_t TimeOut = HSI_VALUE;

	while(((I2Cx->SR1) & Flags) != Flags)
		if (!(TimeOut--))
			return ((I2Cx->SR1 & I2C_SR1_BITMASK) | I2C_ERROR_BIT); /* Set one of the reserved bits to signal timeout */

	return (I2Cx->SR1 & I2C_SR1_BITMASK);
}

static uint16_t WaitLineIdle(I2C_TypeDef *I2Cx)
{
	/* Wait till the Line becomes idle. */

	uint32_t TimeOut = HSI_VALUE;
	/* Check to see if the Line is busy */
	/* This bit is set automatically when a start condition is broadcasted on the line (even from another master) */
	/* and is reset when stop condition is detected. */
	while((I2Cx->SR2) & (I2C_SR2_BUSY))
		if (!(TimeOut--))
			return ((I2Cx->SR1 & I2C_SR1_BITMASK) | I2C_ERROR_BIT); /* Set one of the reserved bits to signal timeout */

	return (I2Cx->SR1 & I2C_SR1_BITMASK);
}
