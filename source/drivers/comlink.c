/* *
 *
 * Hardware Abstraction Layer for all comunication
 *
 * TODO:
 * Add support for Serial Communication AUX1, AUX2 and AUX3
 * Add command parser
 *
 * */

/* *
 *
 * Serial Communication Protocol
 * -----------------------------
 * This was designed so that binary data could be sent while not
 * needing to "code" it as ASCII HEX. A simple SYNC byte is used
 * to denote the start of a transfer and after that a header
 * containing the command and size of the message. If the size is
 * greater than 0 a data package comes after the first CRC. If
 * the data contains a byte that has the same value as the sync
 * byte it will be replaced by two sync bytes "x" -> "xx" to denote
 * a byte of the value sync and not a data package sync.
 *
 *
 * Protocol:
 * 		SYNC | HEADER | CRC8 | DATA | CRC16
 * 		DATA and CRC16 is optional.
 *
 * HEADER:
 * 		CMD 	| DATA SIZE
 * 		1 byte 	| 1 byte
 *
 * DATA:
 * 		BINARY DATA
 * 		1 - 255 bytes
 * or if Firmware Up/Download:
 * 		PACKAGE NO.	| BINARY DATA
 * 		2 bytes		| 1 - 64 bytes
 *
 * SYNC: 1 byte
 * 		Sent once = SYNC
 * 		Sent twice = databyte with the value of SYNC
 *
 * CRC8: 1 byte
 * 		CRC-8 of SYNC and HEADER
 *
 * CRC16: 2 bytes
 * 		CCITT (16-bit) of whole message including SYNC and CRC8
 * 		For more information about the CRCs look in crc.c/crc.h
 *
 * */


#include "comlink.h"

/* Gobal variable defines */
volatile xQueueHandle xUSBQueueHandle = NULL;
volatile Bool bUSBAvalible = FALSE;

/* Private function defines */

/* Private external functions */
extern uint16_t cdc_DataTx(uint8_t *, uint32_t);

/* *
 * vUSBQueueInit initializes the queue for USB recieved data
 * */
void vUSBQueueInit(void)
{
	xUSBQueueHandle = xQueueCreate(xUSBQueueSize, sizeof(uint8_t));
}

/* *
 *
 * Function for sending data over USB.
 * Theoretical max speed is 1.6M baud however a much slower rate is
 * recommened because the in and out endpoints uses the same buffer.
 * 115200 buad is normal operating conditions. (Higher can be used)
 *
 * Please observe that this function is NOT ISR-safe for the RTOS.
 * If called from ISR it can break pakages sent via the RTOS.
 *
 * */
ErrorStatus xUSBSendData(uint8_t *data, uint32_t size)
{
	/*  if USB is not avalible, signal with error. */
	if (bUSBAvalible == FALSE)
		return ERROR;

	else
	{
		vTaskSuspendAll();
		{
			cdc_DataTx(data, size);
		}
		xTaskResumeAll();

		return SUCCESS;
	}
}
