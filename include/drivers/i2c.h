#ifndef __I2C_H
#define __I2C_H

/* Standard includes */
#include "stm32f4xx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Drivers */
#include "comlink.h"

/* KFly includes */

/* Includes */

/* Defines */
/* Status */
#define I2C_STATUS_BITMASK		0x00DF
#define I2C_ERROR_BITMASK		0xDF00
#define I2C_SR1_BITMASK			0xDFDF
#define I2C_ERROR_BIT			((uint16_t)(1<<13)) /* One of the reserved bits is used as timeout error bit */

/* Global variable defines */
extern volatile xQueueHandle 	I2CMutex[3]; /* Mutexes for the I2C */

/* Typedefs */
typedef enum
{
	I2C_TRANSFER_POLLING = 0,	/* Transfer in polling mode */
	I2C_TRANSFER_INTERRUPT		/* Transfer in interrupt mode */
} I2C_TRANSFER_OPTION_Type;

typedef enum
{
	I2C_SENDING = 0,
	I2C_RECEIVING
} I2C_DIRECTION_Type;

typedef struct
{
	uint8_t Slave_Address_7bit;		/* Slave address in 7bit mode */
	uint8_t *TX_Data;				/* Pointer to Transmit data - NULL if data transmit is not used */
	uint8_t TX_Length;				/* Transmit data length - 0 if data transmit is not used*/
	uint8_t TX_Count;				/* Current Transmit data counter */
	uint8_t *RX_Data;				/* Pointer to Receive data - NULL if data receive is not used */
	uint8_t RX_Length;				/* Receive data length - 0 if data receive is not used */
	uint8_t RX_Count;				/* Current Receive data counter */
	uint8_t Retransmissions_Max;	/* Maximum number of retransmissions */
	uint8_t Retransmissions_Count;	/* Current retransmission counter */
	uint16_t Status;				/* Current status of I2C activity */
	void (*Callback)(void);			/* Pointer to "callback" function when transmission is complete. Used in interrupt transfer mode */
} I2C_MASTER_SETUP_Type;


/* Global function defines */
void SensorBusInit(void);
ErrorStatus I2C_MasterTransferData(I2C_TypeDef *, I2C_MASTER_SETUP_Type *, I2C_TRANSFER_OPTION_Type);

#endif
