/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_usr.h"
#include "usbd_ioreq.h"
#include "led.h"
#include "comlink.h"

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  /*
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,
  */
};


/**
* @brief  USBD_USR_Init 
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{  
  /* Initialize LEDs */
  
}

/**
* @brief  USBD_USR_DeviceReset 
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{

}


/**
* @brief  USBD_USR_DeviceConfigured
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConfigured (void)
{
	/* USB status change, tell the system */
	xUSBQueue.bUSBAvalible = TRUE;

	// Debug LED
	LEDOn(GREEN);
}

/**
* @brief  USBD_USR_DeviceSuspended 
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
	/* USB status change, tell the system */
	xUSBQueue.bUSBAvalible = FALSE;

	// Debug LED
	LEDOff(GREEN);
	/* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed 
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
	/* USB status change, tell the system */
	xUSBQueue.bUSBAvalible = TRUE;

	// Debug LED
	LEDOn(GREEN);
    /* Users can do their application actions here for the USB-Reset */
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
