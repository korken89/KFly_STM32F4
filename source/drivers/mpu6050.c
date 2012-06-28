/* *
 *
 * Hardware Abstraction Layer for MPU6050 Accelerometer and Gyro
 *
 * TODO:
 * ...
 *
 * */


#include "mpu6050.h"

/* Gobal variable defines */

/* Private function defines */

/* Private external functions */

void InitMPU6050(void)
{
	/* Setup the sensor */
	uint8_t send[2];
	uint8_t receive;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = send;
	Setup.RX_Data = &receive;
	Setup.Retransmissions_Max = 5;
	Setup.Callback = NULL;
	Setup.TX_Length = 2;
	Setup.RX_Length = 0;

	/* Set internal clock source and Sleep mode */
	send[0] = MPU6050_RA_PWR_MGMT_1; /* Power management register 1*/
	send[1] = 0x01; /* X gyro as referance, sleep disabled */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set Gyro range */
	send[0] = MPU6050_RA_GYRO_CONFIG; /* Gyro configuration register */
	send[1] = 0b00011000; /* range = +-2000 deg/s */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set Accelerometer range */
	send[0] = MPU6050_RA_ACCEL_CONFIG; /* Accel configuration register */
	send[1] = 0b00011000; /* range = +-16g */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set Digital Low-Pass Filter */
	send[0] = MPU6050_RA_CONFIG; /* Configuration register */
	send[1] = 0b00000010; /* DLP level = 2 -> DLP cutoff = ~95Hz*/
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set Sample Rate */
	send[0] = MPU6050_RA_SMPLRT_DIV; /* Sample rate register */
	send[1] = 4; /* Sample at 200Hz -> 1kHz / (1 + SMPLRT) -> SMPLRT = 4 */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set interrupt pin config */
	send[0] = MPU6050_RA_INT_PIN_CFG; /* Interrupt pin register */
	send[1] = 0b00010000; /* INT active high, push-pull, 50us pulse, any read clears interrupt */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);

	/* Set interrupts */
	send[0] = MPU6050_RA_INT_ENABLE; /* Interrupt enable register */
	send[1] = 0x01; /* Data ready interrupt */
	I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);
}

ErrorStatus GetMUP6050ID(uint8_t *data)
{
	static uint8_t send = MPU6050_RA_WHO_AM_I;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = data;
	Setup.RX_Length = 1;
	Setup.Retransmissions_Max = 0;
	Setup.Callback = NULL;

	/*xUSBSendData(data, 1);
	xUSBSendData(&Setup.Status, 2);
	xUSBSendData(&Setup.TX_Count, 1);
	xUSBSendData(&Setup.RX_Count, 1);*/

	return I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_INTERRUPT);
}

ErrorStatus GetMPU6050Rates(uint8_t *data)
{
	static uint8_t send = MPU6050_RA_ACCEL_XOUT_H;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = data;
	Setup.RX_Length = 14;
	Setup.Retransmissions_Max = 0;
	Setup.Callback = NULL;

	return I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);
}
