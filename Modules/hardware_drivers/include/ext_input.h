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
#define ROLE_TO_INDEX_BITS			4
#define ROLE_TO_INDEX_MASK			0x0f


#define CAPTURE_TIMER_RATE 			1000000
#define MAX_NUMBER_OF_INPUTS		12
#define CPPM_SYNC_LIMIT_MIN			3000	/* 3 ms */
#define CPPM_SYNC_LIMIT_MAX			30000	/* 30 ms */
#define RSSI_THRESHOLD_PERCENT		5 		/* RSSI threshold in percent */
#define RSSI_TIMEOUT				100 	/* Number of bad RSSI measurements
											   to reset connection */

#define RC_RAW_INPUT_SIZE			(10 + 2 * MAX_NUMBER_OF_INPUTS)
#define RC_INPUT_SETTINGS_SIZE		(4 + 8 * MAX_NUMBER_OF_INPUTS)

/* Typedefs */
typedef enum {
	INPUT_CH1_CPPM = 0,
	INPUT_CH2_RSSI = 1,
	INPUT_CH3 = 2,
	INPUT_CH4 = 3,
	INPUT_CH5 = 4,
	INPUT_CH6 = 5
} Input_Capture_Channel;

typedef enum {
	MODE_CPPM_INPUT = 1,
	MODE_PWM_INPUT = 2
} Input_Mode_Selector;

typedef enum __attribute__((__packed__)) {
	ROLE_OFF = 0,
	ROLE_THROTTLE = 1,
	ROLE_PITCH = 2,
	ROLE_ROLL = 3,
	ROLE_YAW = 4,
	ROLE_FLIGHT_MODE = 6,
	ROLE_AUX1 = 7,
	ROLE_AUX2 = 8,
	ROLE_AUX3 = 9
} Input_Role_Selector;

typedef enum __attribute__((__packed__)) {
	TYPE_ANALOG = 1,
	TYPE_3_STATE = 2,
	TYPE_ON_OFF = 3
} Input_Type_Selector;

typedef struct {
	Bool active_connection;
	uint16_t number_active_connection;
	uint16_t value[MAX_NUMBER_OF_INPUTS];
	uint16_t rssi;
	uint16_t rssi_frequency;
} Raw_External_Input_Type;

typedef struct {
	Input_Mode_Selector mode;
	Input_Role_Selector role[MAX_NUMBER_OF_INPUTS];
	Input_Type_Selector type[MAX_NUMBER_OF_INPUTS];
	uint16_t ch_center[MAX_NUMBER_OF_INPUTS];
	uint16_t ch_top[MAX_NUMBER_OF_INPUTS];
	uint16_t ch_bottom[MAX_NUMBER_OF_INPUTS];
} Input_Settings_Type;

/* Global functions */
void InputInit(void);
float fGetInputLevel(Input_Role_Selector role);
uint32_t RoleToIndex_CreateLookup(void);
uint32_t RoleToIndex(Input_Role_Selector role);
void Input_CPPM_RSSI_Config(void);
void Input_PWM_Config(void);
Raw_External_Input_Type *ptrGetRCRawInput(void);
Input_Settings_Type *ptrGetRCInputSettings(void);

#endif
