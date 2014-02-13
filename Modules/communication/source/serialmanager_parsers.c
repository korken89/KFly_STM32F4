/* *
 *
 * All the parsers for the different packages.
 * To expand the functionality of the serial com is just to add
 * functions here and add them to the serial com switch-statement.
 *
 * */

#include "stdint.h"
#include "serialmanager_parsers.h"

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

void vGetControllerData(Parser_Holder_Type *pHolder)
{
	uint8_t str[200];
	PI_Data_Type *PI_settings;
	uint8_t *data;
	uint32_t size = 4;
	uint16_t crc16;

	/* Case the control data to an array of PI_Data_Type */
	PI_settings = (PI_Data_Type *)ptrGetControlData();

	str[0] = SYNC_BYTE;
	str[1] = Cmd_GetControllerData;
	str[2] = 144;
	str[3] = CRC8(str, 3);

	for (int i = 0; i < 12; i++)
	{
		data = (uint8_t *)&PI_settings[i];

		for (int j = 0; j < 12; j++)
			str[size++] = data[j];
	}

	crc16 = CRC16(str, size - 1);

	str[size++] = (uint8_t)(crc16 >> 8);
	str[size++] = (uint8_t)(crc16);

	if (pHolder->Port == PORT_USB)
		xUSBSendData(str, size);
}
