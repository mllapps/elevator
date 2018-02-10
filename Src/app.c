/**
 * @file app.c
 * @author fl0mll
 * @date 2017/12/27
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Application implementation
 */
#include "app.h"
#include "tim.h"
#include "eeprom.h"
#include "btn.h"
#include "config.h"
#include "stepper.h"

typedef enum appState_e {
	APP_STATE_IDLE			= 0,
	APP_STATE_DRIVING_UP	= 1,
	APP_STATE_DRIVING_DOWN	= 2
} appState_t;

typedef enum appFloor_e {
	APP_FLOOR_0		= 0,
	APP_FLOOR_1		= 1,
	APP_FLOOR_2		= 2,
} appFloor_t;

typedef struct appData_s {
	/**
	 * Light intensity
	 */
	uint16_t pwmValue;
	/**
	 * Power off value in milliseconds for the
	 */
	uint32_t powerOffTimeMs;
	/**
	 * Number of milliseconds for button trigger interval
	 */
	uint32_t btnTimestamp;
	/**
	 * Number of milliseconds until the LED turns off
	 */
	uint32_t powerTimestamp;
	/**
	 * Number of milliseconds for long press detection
	 */
	uint32_t longpressTime;

	struct {
		appFloor_t current;
		appFloor_t last;
	} floor;

	struct {
		appState_t state;
		appState_t nxState;
	} fsm;
} appData_t;

/**
 * Module data
 */
static appData_t appData;


void app_stateIdle(void);
void app_stateDriveUp(void);
void app_stateDriveDown(void);

/**
 * Initialize the application variables
 *
 * @warning Run the ee_init() function before you run this function.
 */
void app_init()
{
	uint16_t ret = 0;
	uint16_t powerOff;

	UNUSED(ret);

	appData.fsm.state =
	appData.fsm.nxState = APP_STATE_IDLE;

	appData.floor.current =
	appData.floor.last = APP_FLOOR_2;

	appData.pwmValue = 0;

	appData.powerTimestamp =
	appData.btnTimestamp = HAL_GetTick();

	HAL_FLASH_Unlock();

	/* Read the values from the persistent memory */
	ret = EE_ReadVariableOrDefault(
			VirtAddVarTab[CFG_LONGPRESS_TIME_IDX],
			(uint16_t*)&appData.longpressTime,
			CFG_LONGPRESS_TIME_DEFAULT);

	ret = EE_ReadVariableOrDefault(
			VirtAddVarTab[CFG_POWER_OFF_IDX],
			(uint16_t*)&powerOff,
			CFG_POWER_OFF_DEFAULT);

	appData.powerOffTimeMs = 1000 * 60 * powerOff;

	HAL_FLASH_Lock();
}

/**
 * Application handler
 */
void app_handler()
{
	uint32_t curTimeStamp;
	btnRc_t ret;

	/* Trigger the button handler */
	if( (curTimeStamp = HAL_GetTick()) - appData.btnTimestamp >= APP_BUTTON_TRIGGER_INTERVAL) {
		appData.btnTimestamp = curTimeStamp;
		btn_handler();
	}

	switch(appData.fsm.state) {
	case APP_STATE_IDLE:
		app_stateIdle();
		break;
	case APP_STATE_DRIVING_UP:
		app_stateDriveUp();
		break;
	case APP_STATE_DRIVING_DOWN:
		app_stateDriveDown();
		break;
	default:
		break;
	}

	if(appData.fsm.state != appData.fsm.nxState) {
		appData.fsm.state = appData.fsm.nxState;
	}
}

/* Worflow functions --------------------------------------------------------*/

void app_stateIdle(void)
{
	uint32_t curTimeStamp;
	btnRc_t ret;

	/* Perform button action */
	if ( (ret = btn_isPressed() ) == BTN_PRESSED_SHORT)
	{
	   btn_clearAll();

	   HAL_GPIO_TogglePin(LD1_OUT_GPIO_Port, LD1_OUT_Pin);

	   if(appData.floor.current == APP_FLOOR_2) {
		   appData.fsm.nxState = APP_STATE_DRIVING_DOWN;

		   /** @todo drive down to floor 1 */
	   }else if(appData.floor.current == APP_FLOOR_1 && appData.floor.last == APP_FLOOR_2) {
		   appData.fsm.nxState = APP_STATE_DRIVING_DOWN;

		   /** @todo drive down to floor 0 */
	   }else if(appData.floor.current == APP_FLOOR_1 && appData.floor.last == APP_FLOOR_0) {
		   appData.fsm.nxState = APP_STATE_DRIVING_UP;

		   /** @todo drive up to floor 2 */
	   }else if(appData.floor.current == APP_FLOOR_0) {
		   appData.fsm.nxState = APP_STATE_DRIVING_UP;

		   /** @todo drive up to floor 1 */
	   }

	}else if(ret == BTN_PRESSED_LONG) {
		btn_clearLongPress();
	}

	/* Power safe mode detected after N seconds */
	if( (curTimeStamp = HAL_GetTick()) - appData.powerTimestamp >= appData.powerOffTimeMs) {
		appData.powerTimestamp = curTimeStamp;
	}
}

void app_stateDriveUp(void)
{
	/**
	 * @todo Check if the number of steps will be arrived
	 */

}


void app_stateDriveDown(void)
{
	/**
	 * @todo Check if the number of steps will be arrived
	 */
}