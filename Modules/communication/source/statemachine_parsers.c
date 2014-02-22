/* *
 *
 * All the parsers for the different packages.
 * To expand the functionality of the serial com is just to add
 * functions here and add them to the serial com switch-statement.
 *
 * */

#include "stdint.h"
#include "statemachine_parsers.h"

ErrorStatus GenerateMessage(KFly_Command_Type command, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateHeaderOnlyCommand(KFly_Command_Type command, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateGenericCommand(KFly_Command_Type command, uint8_t *data, const uint32_t data_count, Circular_Buffer_Type *Cbuff);
ErrorStatus GenerateACK(Circular_Buffer_Type *Cbuff);
ErrorStatus GeneratePing(Circular_Buffer_Type *Cbuff);

static const Generator_Type generator_lookup[128] = {
	NULL,						/* 0: 	Cmd_None 						*/
	GenerateACK,				/* 1: 	Cmd_ACK 						*/
	GeneratePing,				/* 2: 	Cmd_Ping 						*/
	NULL,						/* 3:	Cmd_DebugMessage				*/
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
	NULL,						/* 17:	Cmd_GetBootloaderVersion		*/
	NULL,						/* 18:	Cmd_GetFirmwareVersion			*/
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
	NULL,						/* 30:	Cmd_GetRateControllerData		*/
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

 /**
  * @brief 			Generate a message based on the Generators in the lookup table.
  * @details 
  * 
  * @param command 	The command to generate a message for.
  * @param Cbuff 	Pointer to the circular buffer to put the data in.
  * 
  * @return 		Return ERROR if the message didn't fit or SUCCESS if it did fit.
  */
ErrorStatus GenerateMessage(KFly_Command_Type command, Circular_Buffer_Type *Cbuff)
{
	ErrorStatus status;

	/* Check so there is an available Generator function for this command */
	if (generator_lookup[command] != NULL)
	{
		/* Claim the circular buffer for writing */
		CircularBuffer_Claim(Cbuff, portMAX_DELAY);
		{
			status = generator_lookup[command](Cbuff);
		}
		CircularBuffer_Release(Cbuff);
		/* Release the circular buffer and return the status */
	}
	else
		status = ERROR;
	
	return status;
}

/**
 * @brief 			Generates a message with no data part.
 * @details
 * 
 * @param command 	Command to generate message for.
 * @param Cbuff 	Pointer to the circular buffer to put the data in.
 * 
 * @return			Return ERROR if the message didn't fit or SUCCESS if it did fit.
 */
ErrorStatus GenerateHeaderOnlyCommand(KFly_Command_Type command, Circular_Buffer_Type *Cbuff)
{
	int32_t count = 0;
	uint8_t crc8;

	/* Write the stating SYNC (without doubling it) */
	CircularBuffer_WriteSYNCNoIncrement(		Cbuff, &count, &crc8, NULL); 

	/* Add all the data to the message */
	CircularBuffer_WriteNoIncrement(command,	Cbuff, &count, &crc8, NULL); 
	CircularBuffer_WriteNoIncrement(0, 			Cbuff, &count, &crc8, NULL); 
	CircularBuffer_WriteNoIncrement(crc8, 		Cbuff, &count, NULL,  NULL);

	/* Check if the message fit inside the buffer */
	return CircularBuffer_Increment(count, Cbuff);	
}

/**
 * @brief 				Generates a message with data and CRC16 part. 
 * @details	
 * 
 * @param command 		Command to generate message for.
 * @param data 			Pointer to where the data is located.
 * @param data_count 	Number of data bytes.
 * @param Cbuff 		Pointer to the circular buffer to put the data in.
 * 
 * @return				Return ERROR if the message didn't fit or SUCCESS if it did fit.
 */
ErrorStatus GenerateGenericCommand(KFly_Command_Type command, uint8_t *data, const uint32_t data_count, Circular_Buffer_Type *Cbuff)
{
	int32_t count = 0;
	uint8_t crc8;
	uint16_t crc16;

	/* Check if the "best case" won't fit in the buffer which is
	 * data_count + header + CRC16 = data_count + 6 bytes */
	if (CircularBuffer_SpaceLeft(Cbuff) < (data_count + 6))
		return ERROR;

	/* Add the header */
	/* Write the starting SYNC (without doubling it) */
	CircularBuffer_WriteSYNCNoIncrement(		Cbuff, &count, &crc8, &crc16); 

	/* Add all of the header to the message */
	CircularBuffer_WriteNoIncrement(command,	Cbuff, &count, &crc8, &crc16); 
	CircularBuffer_WriteNoIncrement(data_count, Cbuff, &count, &crc8, &crc16); 
	CircularBuffer_WriteNoIncrement(crc8, 		Cbuff, &count, NULL,  &crc16);

	/* Add the data to the message */
	for (int i = 0; i < data_count; i++)
		CircularBuffer_WriteNoIncrement(data[i], Cbuff, &count, NULL, &crc16); 

	/* Add the CRC16 */
	CircularBuffer_WriteNoIncrement((uint8_t)(crc16 >> 8), 	Cbuff, &count, NULL, NULL);
	CircularBuffer_WriteNoIncrement((uint8_t)(crc16), 		Cbuff, &count, NULL, NULL);

	return CircularBuffer_Increment(count, Cbuff);	/* Check if the message fit inside the buffer */
}

/**
 * @brief 			Generates an ACK.
 * @details
 * 
 * @param Cbuff 	Pointer to the circular buffer to put the data in.
 * 
 * @return 			Return ERROR if the message didn't fit or SUCCESS if it did fit.
 */
ErrorStatus GenerateACK(Circular_Buffer_Type *Cbuff)
{
	return GenerateHeaderOnlyCommand(Cmd_ACK, Cbuff);	/* Return status */
}

/**
 * @brief 			Generates a Ping.
 * @details
 * 
 * @param Cbuff 	Pointer to the circular buffer to put the data in.
 * 
 * @return 			Return ERROR if the message didn't fit or SUCCESS if it did fit.
 */
ErrorStatus GeneratePing(Circular_Buffer_Type *Cbuff)
{
	return GenerateHeaderOnlyCommand(Cmd_Ping, Cbuff);	/* Return status */
}

/* *
 * Sends a ping.
 * */
void vPing(Parser_Holder_Type *pHolder)
{
	uint8_t str[4];

	str[0] = SYNC_BYTE;
	str[1] = Cmd_Ping;
	str[2] = 0;
	str[3] = CRC8(str, 3);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, 4);
}

/* *
 * Get the current running mode. P for program, B for bootloader.
 * */
void vGetRunningMode(Parser_Holder_Type *pHolder)
{
	uint8_t str[7];
	uint16_t crc16;

	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetRunningMode;
	str[2] = 0;
	str[3] = CRC8(str, 3);
	str[4] = 'P'; /* P for program mode */
	crc16 = CRC16(str, 5);

	str[5] = (uint8_t)(crc16 >> 8);
	str[6] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, 7);
}

/* *
 * Gets the bootloader version and sends it.
 * */
void vGetBootloaderVersion(Parser_Holder_Type *pHolder)
{
	uint8_t *msg;
	msg = (uint8_t *)(BOOTLOADER_BASE + SW_VERSION_OFFSET); /* Text is at known location */
	uint8_t str[128];
	int i = 4;
	uint8_t txt = *msg;
	/* TODO: Add message holder */
	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetBootloaderVersion;

	while (txt && (i < 60)) /* In case there is something wrong with the string  */
	{
		txt = *(msg + i);
		str[i++] = txt;
	}

	str[2] = (uint8_t)i - 4;
	str[3] = CRC8(str, 3);
	uint16_t crc16 = CRC16(str, i);

	str[i++] = (uint8_t)(crc16 >> 8);
	str[i++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i);
}

/* *
 * Gets the firmware version and sends it.
 * */
void vGetFirmwareVersion(Parser_Holder_Type *pHolder)
{
	uint8_t *msg;
	msg = (uint8_t *)(FIRMWARE_BASE + SW_VERSION_OFFSET); /* Text is at known location */
	uint8_t str[128];
	int i = 4;
	uint8_t txt = *msg;
	/* TODO: Add message holder */
	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetFirmwareVersion;

	while (txt && (i < 60)) /* In case there is something wrong with the string  */
	{
		txt = *(msg + i);
		str[i++] = txt;
	}

	str[2] = (uint8_t)i - 4;
	str[3] = CRC8(str, 3);
	uint16_t crc16 = CRC16(str, i);

	str[i++] = (uint8_t)(crc16 >> 8);
	str[i++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, i);
}

void vGetRateControllerData(Parser_Holder_Type *pHolder)
{
	uint8_t str[100];
	PI_Data_Type *PI_settings;
	uint8_t *CL_settings;
	uint8_t *data;
	uint32_t size = 4;
	int i, j;
	uint16_t crc16;

	/* CasT the control data to an array of PI_Data_Type */
	PI_settings = (PI_Data_Type *)ptrGetControlData();

	/* Cast the settings into bytes for read out */
	CL_settings = (uint8_t *)ptrGetControlLimits();

	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetRateControllerData;
	str[2] = 48;
	str[3] = CRC8(str, 3);

	for (i = 0; i < 3; i++) /* Get only the rate coefficients */
	{
		data = (uint8_t *)&PI_settings[i];

		for (j = 0; j < 12; j++)
			str[size++] = data[j];
	}

	for (i = 0; i < 12; i++) /* Get only the rate constraints */
	{
		str[size++] = CL_settings[i];
	}

	crc16 = CRC16(str, size - 1);

	str[size++] = (uint8_t)(crc16 >> 8);
	str[size++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, size);
}
