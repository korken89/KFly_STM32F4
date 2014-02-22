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

#include "statemachine.h"

static const Parser_Type parser_lookup[128] = {
	NULL,						/* 0: 	Cmd_None 						*/
	NULL,						/* 1: 	Cmd_ACK 						*/
	vPing,						/* 2: 	Cmd_Ping 						*/
	vGetRunningMode,			/* 3:	Cmd_DebugMessage				*/
	NULL,						/* 4:	Cmd_GetRunningMode 				*/
	NULL,						/* 5 */
	NULL,						/* 6 */
	NULL,						/* 7 */
	NULL,						/* 8 */
	NULL,						/* 9 */
	NULL,						/* 10:	Cmd_PrepareWriteFirmware 		*/
	NULL,						/* 11: 	Cmd_WriteFirmwarePackage 		*/
	NULL,						/* 12:	Cmd_WriteLastFirmwarePackage	*/
	NULL,						/* 13:	Cmd_ReadFirmwarePackage			*/
	NULL,						/* 14:	Cmd_ReadLastFirmwarePackage		*/
	NULL,						/* 15:	Cmd_NextPackage					*/
	NULL,						/* 16:	Cmd_ExitBootloader				*/
	vGetBootloaderVersion,		/* 17:	Cmd_GetBootloaderVersion		*/
	vGetFirmwareVersion,		/* 18:	Cmd_GetFirmwareVersion			*/
	NULL,						/* 19:	Cmd_SaveToFlash					*/
	NULL,						/* 20 */
	NULL,						/* 21 */
	NULL,						/* 22 */
	NULL,						/* 23 */
	NULL,						/* 24 */
	NULL,						/* 25 */
	NULL,						/* 26 */
	NULL,						/* 27 */
	NULL,						/* 28 */
	NULL,						/* 29 */
	vGetRateControllerData,		/* 30:	Cmd_GetRateControllerData		*/
	NULL,						/* 31:	Cmd_SetRateControllerData		*/
	NULL,						/* 32:	Cmd_GetAttitudeControllerData	*/
	NULL,						/* 33:	Cmd_SetAttitudeControllerData	*/
	NULL,						/* 34:	Cmd_GetVelocityControllerData	*/
	NULL,						/* 35: 	Cmd_SetVelocityControllerData 	*/
	NULL,						/* 36:	Cmd_GetPositionControllerData	*/
	NULL,						/* 37:	Cmd_SetPositionControllerData	*/
	NULL,						/* 38:	RESERVED						*/
	NULL,						/* 39:	Cmd_GetChannelMix				*/
	NULL,						/* 40:	Cmd_SetChannelMix				*/
	NULL,						/* 41:	Cmd_GetRCCalibration			*/
	NULL,						/* 42:	Cmd_SetRCCalibration			*/
	NULL,						/* 43:	Cmd_GetRCValues					*/
	NULL,						/* 44:	Cmd_GetSensorData				*/
	NULL,						/* 45 */
	NULL,						/* 46 */
	NULL,						/* 47 */
	NULL,						/* 48 */
	NULL,						/* 49 */
	NULL,						/* 50 */
	NULL,						/* 51 */
	NULL,						/* 52 */
	NULL,						/* 53 */
	NULL,						/* 54 */
	NULL,						/* 55 */
	NULL,						/* 56 */
	NULL,						/* 57 */
	NULL,						/* 58 */
	NULL,						/* 59 */
	NULL,						/* 60 */
	NULL,						/* 61 */
	NULL,						/* 62 */
	NULL,						/* 63 */
	NULL,						/* 64 */
	NULL,						/* 65 */
	NULL,						/* 66 */
	NULL,						/* 67 */
	NULL,						/* 68 */
	NULL,						/* 69 */
	NULL,						/* 70 */
	NULL,						/* 71 */
	NULL,						/* 72 */
	NULL,						/* 73 */
	NULL,						/* 74 */
	NULL,						/* 75 */
	NULL,						/* 76 */
	NULL,						/* 77 */
	NULL,						/* 78 */
	NULL,						/* 79 */
	NULL,						/* 80 */
	NULL,						/* 81 */
	NULL,						/* 82 */
	NULL,						/* 83 */
	NULL,						/* 84 */
	NULL,						/* 85 */
	NULL,						/* 86 */
	NULL,						/* 87 */
	NULL,						/* 88 */
	NULL,						/* 89 */
	NULL,						/* 90 */
	NULL,						/* 91 */
	NULL,						/* 92 */
	NULL,						/* 93 */
	NULL,						/* 94 */
	NULL,						/* 95 */
	NULL,						/* 96 */
	NULL,						/* 97 */
	NULL,						/* 98 */
	NULL,						/* 99 */
	NULL,						/* 100 */
	NULL,						/* 101 */
	NULL,						/* 102 */
	NULL,						/* 103 */
	NULL,						/* 104 */
	NULL,						/* 105 */
	NULL,						/* 106 */
	NULL,						/* 107 */
	NULL,						/* 108 */
	NULL,						/* 109 */
	NULL,						/* 110 */
	NULL,						/* 111 */
	NULL,						/* 112 */
	NULL,						/* 113 */
	NULL,						/* 114 */
	NULL,						/* 115 */
	NULL,						/* 116 */
	NULL,						/* 117 */
	NULL,						/* 118 */
	NULL,						/* 119 */
	NULL,						/* 120 */
	NULL,						/* 121 */
	NULL,						/* 122 */
	NULL,						/* 123 */
	NULL,						/* 124 */
	NULL,						/* 125 */
	NULL,						/* 126 */
	NULL						/* 127 */
};

/* *
 *
 * ---------------- State machine functions ----------------
 * State machine's different states (functions) starts below.
 * NO (!) other functions are allowed there!
 *
 * */

/* *
 * The entry point of serial data.
 * */
void vStatemachineDataEntry(uint8_t data, Parser_Holder_Type *pHolder)
{
	if (data == SYNC_BYTE)
	{
		if ((pHolder->next_state != vWaitingForSYNC) && \
		    (pHolder->next_state != vWaitingForSYNCorCMD) && \
		    (pHolder->next_state != vRxCmd))
		{
			pHolder->current_state = pHolder->next_state;
			pHolder->next_state = vWaitingForSYNCorCMD;
		}
		else
			pHolder->next_state(data, pHolder);
	}
	else
		pHolder->next_state(data, pHolder);
}


/* *
 * Waiting for SYNC function. Will run this until a valid SYNC has occurred.
 * */
void vWaitingForSYNC(uint8_t data, Parser_Holder_Type *pHolder)
{
	if (data == SYNC_BYTE)
	{
		pHolder->buffer_count = 0;
		pHolder->buffer[pHolder->buffer_count++] = SYNC_BYTE;
		pHolder->next_state = vRxCmd;
	}
}

/* *
 * A SYNC appeared in data stream.
 * When a SYNC is detected in the data stream the program will
 * look and see if it is an SYNC byte or if it's a new SYNC.
 *
 * */
void vWaitingForSYNCorCMD(uint8_t data, Parser_Holder_Type *pHolder)
{
	if (data == SYNC_BYTE) /* Byte with value of SYNC received,	send it to the function waiting for a byte */
		pHolder->current_state(data, pHolder);
	else /* If not SYNC, reset transfer and check if byte is command */
	{
		pHolder->buffer_count = 0;
		pHolder->buffer[pHolder->buffer_count++] = SYNC_BYTE;
		vRxCmd(data, pHolder);
	}
}

/* *
 * Command parser. Checks if a valid command was received.
 * */
void vRxCmd(uint8_t data, Parser_Holder_Type *pHolder)
{
	/* 0 is not an allowed command (Cmd_None) */
	if ((data & ~ACK_BIT) > Cmd_None)
	{
		pHolder->next_state = vRxSize;
		pHolder->buffer[pHolder->buffer_count++] = data;

		/* Get the correct parser from the parser lookup table */
		pHolder->parser = parser_lookup[(data & ~ACK_BIT)];
	}
	else
	{
		pHolder->next_state = vWaitingForSYNC;
		pHolder->rx_error++;
	}
}

/* *
 * Checks the length of a message.
 * */
void vRxSize(uint8_t data, Parser_Holder_Type *pHolder)
{
	pHolder->next_state = vRxCRC8;
	pHolder->buffer[pHolder->buffer_count++] = data;
	pHolder->data_length = data; /* Set the length of the message to that of the header. */
}

/* *
 * Checks the Header CRC8.
 * */
void vRxCRC8(uint8_t data, Parser_Holder_Type *pHolder)
{
	pHolder->buffer[pHolder->buffer_count++] = data;

	if (CRC8(pHolder->buffer, 3) == data)
	{
		/* CRC OK! */

		if (pHolder->data_length == 0)
		{	/* If no data, parse now! */
			pHolder->next_state = vWaitingForSYNC;

			/* If ACK is requested */
			if (pHolder->buffer[1] & ACK_BIT)
				pHolder->AckRequested = TRUE;
			else
				pHolder->AckRequested = FALSE;

			if (pHolder->parser != NULL)
				pHolder->parser(pHolder);
		}
		else
			pHolder->next_state = vRxData;
	}
	else /* CRC error! */
	{
		pHolder->next_state = vWaitingForSYNC;
		pHolder->rx_error++;
	}
}

/* *
 * Data receiver function. Keeps track of the number of received bytes
 * and decides when to check for CRC.
 * */
void vRxData(uint8_t data, Parser_Holder_Type *pHolder)
{
	pHolder->buffer[pHolder->buffer_count++] = data;

	if (pHolder->buffer_count < (pHolder->data_length + 4))
		pHolder->next_state = vRxData;
	else
		pHolder->next_state = vRxCRC16;
}
/* *
 * Checks the whole message for errors and calls the parser if the
 * message was without errors.
 * */
void vRxCRC16(uint8_t data, Parser_Holder_Type *pHolder)
{
	pHolder->buffer[pHolder->buffer_count++] = data;

	if (pHolder->buffer_count < (pHolder->data_length + 6))
		pHolder->next_state = vRxCRC16;

	else
	{
		pHolder->next_state = vWaitingForSYNC;
		/* Cast the 2 bytes containing the CRC-CCITT to a 16-bit variable */
		uint16_t crc = ((uint16_t)(pHolder->buffer[pHolder->buffer_count - 2]) << 8) | (uint16_t)pHolder->buffer[pHolder->buffer_count - 1];

		if (CRC16(pHolder->buffer, (pHolder->buffer_count - 2)) == crc)
		{
			/* If ACK is requested */
			if (pHolder->buffer[1] & ACK_BIT)
				pHolder->AckRequested = TRUE;
			else
				pHolder->AckRequested = FALSE;

			/* If there is a parser for the message, execute it */
			if (pHolder->parser != NULL)
				pHolder->parser(pHolder);
		}
		else /* CRC error! Discard data. */
		{
			pHolder->next_state = vWaitingForSYNC;
			pHolder->rx_error++;
		}
	}
}
