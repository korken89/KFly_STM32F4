/* *
 *
 * OS layer for Serial Communication.
 * Handles package coding/decoding.
 *
 * ----------!!!----------
 * A warning to those who wants to read the code:
 * This is an orgie of function pointers and buffers.
 * ----------!!!----------
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
 * 		Sent twice = data byte with the value of SYNC ("escaping")
 *
 * CRC8: 1 byte
 * 		CRC-8 of SYNC and HEADER
 *
 * CRC16: 2 bytes
 * 		CCITT (16-bit) of whole message including SYNC and CRC8
 * 		For more information about the CRCs look in crc.c/crc.h
 *
 *
 * -------------------- OBSERVE! --------------------
 * A command is build up by 8 bits (1 byte) and the 8-th bit is the ACK Request bit.
 * So all command must not use the ACK bit unless they need an ACK.
 * Command 0bAxxx xxxx <- A is ACK-bit.
 * Also the value 0xa6/166d/0b10100110 is reserved as the SYNC-byte.
 *
 * This gives 126 commands for the user.
 *
 * */

#include "serialmanager.h"

/* *
 *
 * Initializes all communication.
 *
 * */
void vSerialManagerInit(void)
{
	vUSBQueueInit();

	xTaskCreate(vTaskUSBSerialManager,
				"SerialManager (USB)",
				256,
				0,
				tskSerialManagerPRIORITY,
			    0);
}


/* *
 *
 * The Serial Manager task will handle incoming
 * data and direct it for decode and processing.
 *
 * */
void vTaskUSBSerialManager(void *pvParameters)
{
	uint8_t in_data;
	Parser_Holder_Type data_holder;
	static uint8_t buffer[SERIAL_BUFFER_SIZE]; /* Buffer for serial USB commands */

	/* Initialize data structure */
	data_holder.Port = PORT_USB;
	data_holder.buffer = buffer;
	data_holder.current_state = NULL;
	data_holder.next_state = vWaitingForSYNC;
	data_holder.parser = NULL;
	data_holder.rx_error = 0;

	while(1)
	{
		xQueueReceive(xUSBQueue.xUSBQueueHandle, &in_data, portMAX_DELAY);

		vStatemachineDataEntry(in_data, &data_holder);
	}
}
