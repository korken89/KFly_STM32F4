/* *
 *
 * OS layer for Serial Communication.
 * Handles package coding/decoding.
 *
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

#include "serialmanager.h"

/* *
 *
 * Initializes all communication.
 *
 * */
void vInitSerialManager(void)
{
	vUSBQueueInit();


	xTaskCreate(vTaskUSBSerialManager,
				"SerialManager(USB)",
				512,
				0,
				tskSerialManagerPRIORITY,
			    0);
}


/* *
 *
 * The Serial Manager task will handle incomming
 * data and direct it for decode and processing.
 * The state machine can be seen in serial_state.png
 *
 * */
void vTaskUSBSerialManager(void *pvParameters)
{
	char in_data;
	Parser_Holder_Type data_holder;

	data_holder.Port = PORT_USB;
	data_holder.current_state = NULL;
	data_holder.next_state = vWaitingForSYNC;
	data_holder.parser = NULL;
	data_holder.rx_error = 0;

	while(1)
	{
		xQueueReceive(xUSBQueue.xUSBQueueHandle, &in_data, portMAX_DELAY);
		data_holder.next_state(in_data, &data_holder); /* This should only run when a correct byte/double sync has occured */
	}
}

void vWaitingForSYNC(uint8_t data, Parser_Holder_Type *pHolder)
{

}

void vWaitingForCMD(uint8_t data, Parser_Holder_Type *pHolder)
{

}

void vWaitingForSYNCorCMD(uint8_t data, Parser_Holder_Type *pHolder)
{

}

void vRxCmd(uint8_t data, Parser_Holder_Type *pHolder)
{
	switch (data & 0x7F)
	{
		case Ping:
			pHolder->current_state = vRxCmd;
			pHolder->next_state = NULL;
			pHolder->parser = NULL;
			pHolder->data_length = PingLength;
			break;

		default:
			pHolder->current_state = NULL;
			pHolder->next_state = vWaitingForSYNC;
			pHolder->parser = NULL;
			break;
	}
}

