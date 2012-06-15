#ifndef __I2C_H
#define __I2C_H

/* Standard includes */
#include "stm32f4xx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* KFly includes */

/* Includes */

/* Defines */

/* Global variable defines */

/* Typedefs */
typedef struct
{
	uint32_t	Slave_Address_7bit;		/* Slave address in 7bit mode */
	uint8_t		*TX_Data;				/* Pointer to Transmit data - NULL if data transmit is not used */
	uint32_t	TX_Length;				/* Transmit data length - 0 if data transmit is not used*/
	uint32_t	TX_Count;				/* Current Transmit data counter */
	uint8_t		*RX_Data;				/* Pointer to Receive data - NULL if data receive is not used */
	uint32_t	RX_Length;				/* Receive data length - 0 if data receive is not used */
	uint32_t	RX_Count;				/* Current Receive data counter */
	uint32_t	Retransmissions_Max;	/* Max Re-Transmission value */
	uint32_t	Retransmissions_Count;	/* Current Re-Transmission counter */
	uint16_t	Status;					/* Current status of I2C activity */
	void 		(*Callback)(void);		/* Pointer to Call back function when transmission complete used in interrupt transfer mode */
} I2C_MASTER_SETUP_Type;

typedef enum
{
	I2C_TRANSFER_POLLING = 0,	/* Transfer in polling mode */
	I2C_TRANSFER_INTERRUPT		/* Transfer in interrupt mode */
} I2C_TRANSFER_OPTION_Type;
/* Global function defines */


#endif
