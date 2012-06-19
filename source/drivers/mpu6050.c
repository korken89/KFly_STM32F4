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

}

ErrorStatus GetMUP6050ID(uint8_t *data)
{
	uint8_t send = 0x75;
	I2C_MASTER_SETUP_Type Setup;

	Setup.Slave_Address_7bit = MPU6050_ADDRESS;
	Setup.TX_Data = &send;
	Setup.TX_Length = 1;
	Setup.RX_Data = data;
	Setup.RX_Length = 1;
	Setup.Retransmissions_Max = 0;
	ErrorStatus temp = I2C_MasterTransferData(I2C2, &Setup, I2C_TRANSFER_POLLING);


	/*xUSBSendData(data, 1);
	xUSBSendData(&Setup.Status, 2);
	xUSBSendData(&Setup.TX_Count, 1);
	xUSBSendData(&Setup.RX_Count, 1);*/


	return temp;
}
