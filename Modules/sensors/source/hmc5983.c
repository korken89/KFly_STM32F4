/* *
 *
 * Hardware Abstraction Layer for HMC5983 Magnetometer
 *
 * TODO:
 * Everything
 *
 * */


#include "HMC5983.h"

/* Global variable defines */

/* Private function defines */

/* Private external functions */


void HMC5983Init(void)
{
	/* Setup the sensor */
	uint8_t send[2];
	uint8_t receive;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = HMC5983_ADDRESS;
	Setup.TX_Data = send;
	Setup.RX_Data = &receive;
	Setup.Retransmissions_Max = 5;
	Setup.Callback = NULL;
	Setup.TX_Length = 2;
	Setup.RX_Length = 0;

	/* Set averaging, update rate and bias */
	send[0] = HMC5983_RA_CONFIG_A;
	send[1] = 0b01111000; /* 8 averages, 75Hz, normal mode */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set the gain */
	send[0] = HMC5983_RA_CONFIG_B;
	send[1] = 0b00100000; /* Default gain of 1090 LSB/Gauss */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

    /* Set the mode, continuous or single */
	send[0] = HMC5983_RA_MODE;
	send[1] = 0; /* Continuous measurement mode */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);
}

ErrorStatus GetHMC5983ID(uint8_t *data)
{
	static uint8_t send = HMC5983_RA_ID_A;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = HMC5983_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = data;
	Setup.RX_Length = 3;
	Setup.Retransmissions_Max = 0;
	Setup.Callback = NULL;

	return I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);
}
