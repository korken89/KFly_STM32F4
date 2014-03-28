#ifndef __EXT_INPUT_H
#define __EXT_INPUT_H

/* Standard includes */
#include "stm32f4xx.h"

/* System includes */

/* Scheduler includes. */

/* KFly includes */

/* Driver Includes */

/* Includes */

/* Defines */
#define MAX_NUMBEROFINPUTS		12

/* Typedefs */
typedef struct {
	Bool active_connection;
	int16_t value[MAX_NUMBEROFINPUTS];
	float rssi;
} Raw_External_Input_Type;

/* Global functions */
void Input_CPPM_RSSI_Config(void);
void Input_PWM_Config(void);

#endif
