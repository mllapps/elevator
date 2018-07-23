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
#include "io.h"

/* MLOG settings for the module app */
#define MLOG_DEBUG			(0x01)
#define MLOG_INFO			(0x02)
#define MLOG_WARNING		(0x04)

#include <mlog.h>

/**
 * Application state enumeration type
 */
typedef enum appState_e {
	APP_STATE_INIT			    = 0,
	APP_STATE_IDLE			    = 1,
	APP_STATE_DRIVING_UP	    = 2,
	APP_STATE_DRIVING_DOWN	    = 3,

	APP_STATE_SETUP_INIT        = 20,
    APP_STATE_SETUP_FLOOR2_1    = 21,
    APP_STATE_SETUP_FLOOR1_0    = 22,
} appState_t;

/**
 * Floor enumeration type
 */
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

		uint32_t level1_2;
		uint32_t level0_1;
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

/* Forward declarations ------------------------------------------------------*/

void app_stateInit(void);
void app_stateIdle(void);
void app_stateDriveUp(void);
void app_stateDriveDown(void);
void app_stateSetupInit(void);
void app_stateSetupFloor21(void);
void app_stateSetupFloor10(void);

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
			appData.fsm.nxState = APP_STATE_INIT;

	appData.floor.current =
			appData.floor.last = APP_FLOOR_2;

	appData.pwmValue = 0;

	appData.powerTimestamp =
			appData.btnTimestamp = HAL_GetTick();

	HAL_FLASH_Unlock();

	/* Read the values from the persistent memory */
	ret = ee_readVariableOrDefault(
			VirtAddVarTab[CFG_LONGPRESS_TIME_IDX],
			(uint16_t*)&appData.longpressTime,
			CFG_LONGPRESS_TIME_DEFAULT);

	ret = ee_readVariableOrDefault(
			VirtAddVarTab[CFG_POWER_OFF_IDX],
			(uint16_t*)&powerOff,
			CFG_POWER_OFF_DEFAULT);

	appData.powerOffTimeMs = 1000 * 60 * powerOff;

	/* */
	ret = ee_readVariableOrDefault(
			VirtAddVarTab[CFG_FLOOR_0_1_TICKS_IDX],
			(uint16_t*)&appData.floor.level0_1,
			CFG_FLOOR_0_1_TICKS_DEFAULT);

	/* */
	ret = ee_readVariableOrDefault(
			VirtAddVarTab[CFG_FLOOR_1_2_TICKS_IDX],
			(uint16_t*)&appData.floor.level1_2,
			CFG_FLOOR_1_2_TICKS_DEFAULT);

	stp_setPeriodStartRamp(65535);
	stp_setPeriodEndRamp(45000);

	HAL_FLASH_Lock();

    /* If switch 1 enabled while power on it will enter the setup mode */
    if( io_isSw1() )
    {
        mDebug("Setup mode enabled\n");
        appData.fsm.nxState = APP_STATE_SETUP_INIT;
        return;
    }
}

/**
 * Application handler
 */
void app_handler()
{
	uint32_t curTimeStamp;

	/* Trigger the button handler */
	if( (curTimeStamp = HAL_GetTick()) - appData.btnTimestamp >= APP_BUTTON_TRIGGER_INTERVAL) {
		appData.btnTimestamp = curTimeStamp;
		btn_handler();
	}

	switch(appData.fsm.state) {
	case APP_STATE_INIT:
		app_stateInit();
		break;
	case APP_STATE_IDLE:
		app_stateIdle();
		break;
	case APP_STATE_DRIVING_UP:
		app_stateDriveUp();
		break;
	case APP_STATE_DRIVING_DOWN:
		app_stateDriveDown();
		break;




	case APP_STATE_SETUP_INIT:
	    app_stateSetupInit();
	    break;
	case APP_STATE_SETUP_FLOOR2_1:
        app_stateSetupFloor21();
	    break;
	case APP_STATE_SETUP_FLOOR1_0:
        app_stateSetupFloor10();
	    break;
	default:
		break;
	}

	if(appData.fsm.state != appData.fsm.nxState)
	{
		appData.fsm.state = appData.fsm.nxState;
	}

	/* Power safe mode detected after N seconds */
	if( (curTimeStamp = HAL_GetTick()) - appData.powerTimestamp >= appData.powerOffTimeMs)
	{
		appData.powerTimestamp = curTimeStamp;
		appData.fsm.nxState = APP_STATE_IDLE;

		appData.floor.current = APP_FLOOR_0;
		appData.floor.last = APP_FLOOR_1;

		mInfo("power safe enabled\n");
	}
}

/* Workflow functions --------------------------------------------------------*/

/**
 * @brief Initialize the application and elevator position
 */
void app_stateInit(void)
{
	static int entered = 0;

	if(!entered)
	{
		entered = 1;
		stp_requ(STP_CMD_DRIVE_UP, 16000);
	}

	/* If idle position arrived we want to stop the stepper and go to idle
	 * state
	 */
	if( io_isSw2() )
	{
		stp_requStopFast();

		mDebug("elevator is in idle position\n");
		appData.fsm.nxState = APP_STATE_IDLE;
	}
}

/**
 * @brief Ready for driving to the next position
 */
void app_stateIdle(void)
{
	btnRc_t ret;

	/* Perform button action */
	if ( (ret = btn_isPressed() ) == BTN_PRESSED_SHORT)
	{
		btn_clearAll();

		io_setLd1();

		if(appData.floor.current == APP_FLOOR_2) {
			appData.fsm.nxState = APP_STATE_DRIVING_DOWN;

			stp_requ(STP_CMD_DRIVE_DOWN, appData.floor.level1_2);

			appData.floor.current = APP_FLOOR_1;
			appData.floor.last = APP_FLOOR_2;

			mInfo("drive down to floor 1\n");
		}else if(appData.floor.current == APP_FLOOR_1 && appData.floor.last == APP_FLOOR_2) {
			appData.fsm.nxState = APP_STATE_DRIVING_DOWN;

			stp_requ(STP_CMD_DRIVE_DOWN, appData.floor.level0_1);
			appData.floor.current = APP_FLOOR_0;
			appData.floor.last = APP_FLOOR_1;

			mInfo("drive down to floor 0\n");
		}else if(appData.floor.current == APP_FLOOR_1 && appData.floor.last == APP_FLOOR_0) {
			appData.fsm.nxState = APP_STATE_DRIVING_UP;

			stp_requ(STP_CMD_DRIVE_UP, appData.floor.level1_2);
			appData.floor.current = APP_FLOOR_2;
			appData.floor.last = APP_FLOOR_1;

			mInfo("drive up to floor 2\n");
		}else if(appData.floor.current == APP_FLOOR_0) {
			appData.fsm.nxState = APP_STATE_DRIVING_UP;

			stp_requ(STP_CMD_DRIVE_UP, appData.floor.level0_1);
			appData.floor.current = APP_FLOOR_1;
			appData.floor.last = APP_FLOOR_0;

			mInfo("drive up to floor 1\n");
		}

	}else if(ret == BTN_PRESSED_LONG) {
		btn_clearLongPress();
	}
}

/**
 * @brief Driving to the next upper position
 */
void app_stateDriveUp(void)
{
	stpState_t state;

	if( (state = stp_getState() ) == STP_STATE_ARRIVED) {
		appData.fsm.nxState = APP_STATE_IDLE;

		io_clrLd1();
	}

	/* Stop the drive if idle position has been arrived */
	if(HAL_GPIO_ReadPin(SW2_IN_GPIO_Port, SW2_IN_Pin) == GPIO_PIN_RESET)
	{
		stp_requStopFast();

		mDebug("idle position arrived!\n");
		appData.fsm.nxState = APP_STATE_IDLE;
	}
}

/**
 * @brief Driving to the next lower position
 */
void app_stateDriveDown(void)
{
	stpState_t state;

	if( (state = stp_getState() ) == STP_STATE_ARRIVED) {
		appData.fsm.nxState = APP_STATE_IDLE;
		io_clrLd1();
	}


    /* Stop the drive if idle position has been arrived. This should never be
     * arrived but for security reasons we check this at this state also.
     */
    if(HAL_GPIO_ReadPin(SW2_IN_GPIO_Port, SW2_IN_Pin) == GPIO_PIN_RESET)
    {
        stp_requStopFast();

        mFatal("[fatal] idle position arrived!\n");
        appData.fsm.nxState = APP_STATE_IDLE;
    }
}

/* SETUP ASSISTANT -----------------------------------------------------------*/

/**
 * Setup state to initialize the setup assistant
 */
void app_stateSetupInit()
{

    /* If switch 1 enabled while power on it will enter the setup mode */
    if( io_isSw2() )
    {
        mDebug("Setup idle position arrived\n");
        appData.fsm.nxState = APP_STATE_SETUP_FLOOR2_1;
    }
}

/**
 * Setup state to configure the step count from floor 2 to 1
 */
void app_stateSetupFloor21(void)
{
    btnRc_t ret;
    static uint16_t cnt = 0;
    const uint16_t cntStep = 100;

    /* Perform button action */
    if ( (ret = btn_isPressed() ) == BTN_PRESSED_SHORT)
    {
        btn_clearAll();

        io_setLd1();

        cnt += cntStep;
        stp_requ(STP_CMD_DRIVE_DOWN, cntStep);

    }else if(ret == BTN_PRESSED_LONG) {
        btn_clearLongPress();

        ee_writeVariable(CFG_FLOOR_1_2_TICKS_VADDR, cnt);

        appData.fsm.nxState = APP_STATE_SETUP_FLOOR1_0;
    }
}

/**
 * Setup state to configure the step count from floor 1 to 0
 */
void app_stateSetupFloor10(void)
{
    btnRc_t ret;
    static uint16_t cnt = 0;
    const uint16_t cntStep = 100;

    /* Perform button action */
    if ( (ret = btn_isPressed() ) == BTN_PRESSED_SHORT)
    {
        btn_clearAll();

        io_setLd1();

        cnt += cntStep;
        stp_requ(STP_CMD_DRIVE_DOWN, cntStep);

    }else if(ret == BTN_PRESSED_LONG) {
        btn_clearLongPress();

        ee_writeVariable(CFG_FLOOR_0_1_TICKS_VADDR, cnt);

        appData.fsm.nxState = APP_STATE_INIT;
    }
}

