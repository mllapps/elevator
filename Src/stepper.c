/**
 * @file stepper.c
 * @author fl0mll
 * @date 2018/02/09
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Stepper motor control implementation
 */

#include "stepper.h"
#include "tim.h"
#include "time.h"
#include "io.h"

//#define MLOG_DEBUG			(0x01)
#define MLOG_INFO			(0x02)
#define MLOG_WARNING		(0x04)

#include "mlog.h"

typedef enum stpDecayMode_e {
	STP_DECAY_MODE_FULLSTEP 		= 0,
	STP_DECAY_MODE_HALFSTEP,
	STP_DECAY_MODE_QUARTERSTEP,
	STP_DECAY_MODE_EIGHTSTEP,
	STP_DECAY_MODE_SIXTEENTHSTEP,
} stpDecayMode_t;


typedef struct stpData_s {

	struct {
		stpState_t state;
		stpState_t nxState;
	}fsm;

	struct {
		stpCmd_t active;
		stpCmd_t nxt;
	} cmd;

	struct {
		uint32_t cnt;
		uint32_t target;
	}steps;

	struct {
		uint32_t val;
		uint32_t min;
		uint32_t max;
		uint32_t step;
	} period;

} stpData_t;

static stpData_t stpData;

void stp_setDecayMode(stpDecayMode_t mode);
stpState_t stp_getState(void);

/**
 * @brief Initialize the motor driver and all module variables with default values
 */
void stp_init(void)
{
	stpData.fsm.state =
	stpData.fsm.nxState = STP_STATE_IDLE;

	stpData.steps.cnt =
	stpData.steps.target = 0;

	stpData.cmd.active =
	stpData.cmd.nxt = STP_CMD_NONE;

	stpData.period.val =
	stpData.period.min = 35000;
	stpData.period.max = stpData.period.min/2;
	stpData.period.step = 10;

	stp_setDecayMode(STP_DECAY_MODE_EIGHTSTEP);

	io_clrStpEnable();

	io_clrStpReset();

    io_clrStpSleep();

	HAL_TIM_OC_Init(&htim3);
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
}

/**
 * @brief (De)initialize the stepper driver and stop the motor
 */
void stp_deinit(void)
{
	/* Disable the timer */
	HAL_TIM_Base_Stop(&htim3);

	/* Disable the timer interrupt */
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);

	/* Disable the driver */
	io_clrStpEnable();

	/* Enable sleep mode */
	io_setStpSleep();
}

/**
 * @brief Stepper motor handler
 *
 * Run this handler at your main loop.
 */
void stp_handler(void)
{
	switch(stpData.fsm.state){
	case STP_STATE_ARRIVED:
	case STP_STATE_IDLE:
		/* Disable the timer */
		HAL_TIM_Base_Stop(&htim3);

		/* Disable the driver */
#if 0
		io_clrStpEnable();
		io_setStpSleep();
#endif /* 0 */

		__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);

		if(stpData.cmd.active == STP_CMD_DRIVE_UP || stpData.cmd.active == STP_CMD_DRIVE_DOWN) {
			stpData.fsm.nxState = STP_STATE_RAMP_START;
		}
		break;

	case STP_STATE_RAMP_START:

		if(stpData.cmd.active == STP_CMD_DRIVE_UP) {
			/* set direction to UP */
			HAL_GPIO_WritePin(MTR_DIR_GPIO_Port, MTR_DIR_Pin, GPIO_PIN_SET);

		} else if(stpData.cmd.active == STP_CMD_DRIVE_DOWN) {
			/* set direction to DOWN */
			HAL_GPIO_WritePin(MTR_DIR_GPIO_Port, MTR_DIR_Pin, GPIO_PIN_RESET);
		}else {
			/* Error */
			stpData.fsm.nxState = STP_STATE_FAULT_INVALID_DIR;
		}

		stpData.cmd.nxt =
		stpData.cmd.active = STP_CMD_NONE;

		stpData.steps.cnt = 0;

		stpData.period.val = stpData.period.min;

		/* Enable motor driver  */
		io_setStpEnable();
		io_clrStpSleep();

		/* Enable the timer */
		HAL_TIM_Base_Start(&htim3);

		/* Enable the timer interrupt */
		__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);

		stpData.fsm.nxState = STP_STATE_RAMP_UP;

		break;
	case STP_STATE_RAMP_UP:

		/* Transitions */
		if(stpData.cmd.active == STP_CMD_STOP)
		{
			stpData.fsm.nxState = STP_STATE_IDLE;
		}else if(stpData.steps.cnt >= stpData.steps.target)
		{
			stpData.fsm.nxState = STP_STATE_ARRIVED;
		}
		break;
	case STP_STATE_RAMP_STABLE:

		/* Transitions */
		if(stpData.cmd.active == STP_CMD_STOP)
		{
			stpData.fsm.nxState = STP_STATE_IDLE;
		}else if(stpData.steps.cnt >= stpData.steps.target)
		{
			stpData.fsm.nxState = STP_STATE_ARRIVED;
		}
		break;

	case STP_STATE_FAULT:
	case STP_STATE_FAULT_INVALID_DIR:
		/* Disable the driver */
	    io_clrStpEnable();
	    io_setStpSleep();
		break;
	default:
		break;
	}

	/* Go to next state if requested */
	if(stpData.fsm.state != stpData.fsm.nxState) {
		stpData.fsm.state = stpData.fsm.nxState;

		mDebug("state changed to %d\n", stpData.fsm.state);
	}

	/* New command requested */
	if(stpData.cmd.active != stpData.cmd.nxt) {
		stpData.cmd.active = stpData.cmd.nxt;
	}
}

/**
 * @brief Set the decay mode of the stepper driver
 *
 * MS1 MS2 MS3
 * ===========
 *  0 | 0 | 0 => full step
 *  1 | 0 | 0 => half step
 *  0 | 1 | 0 => quarter step
 *  1 | 1 | 0 => eight step
 *  1 | 1 | 1 => sixteenth step
 */
void stp_setDecayMode(stpDecayMode_t mode)
{
	/* Setup the decay mode */
    io_clrMtrMs1();
    io_clrMtrMs2();
    io_clrMtrMs3();

	switch(mode)
	{
	case STP_DECAY_MODE_FULLSTEP:
		break;
	case STP_DECAY_MODE_HALFSTEP:
	    io_setMtrMs1();
		break;
	case STP_DECAY_MODE_QUARTERSTEP:
        io_setMtrMs2();
		break;
	case STP_DECAY_MODE_EIGHTSTEP:
        io_setMtrMs1();
        io_setMtrMs2();
		break;
	case STP_DECAY_MODE_SIXTEENTHSTEP:
        io_setMtrMs1();
        io_setMtrMs2();
        io_setMtrMs3();
		break;
	default:
	    /* Nothing to do */
		break;
	}
}

void stp_requ(stpCmd_t cmd, uint32_t steps)
{
	stpData.cmd.nxt = cmd;

	stpData.steps.cnt = 0;
	stpData.steps.target = steps;
}

void stp_requStopFast(void)
{
    /* Disable the driver */
#if 0
    io_clrStpEnable();
    io_setStpSleep();
#endif /* 0 */


	/* Disable the timer */
	HAL_TIM_Base_Stop(&htim3);

	/* Disable the timer interrupt */
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);

	stpData.cmd.nxt = STP_CMD_STOP;

	stpData.steps.cnt = 0;
	stpData.steps.target = 0;
}

/*------------------------------------------------------------------------------
 * SETTER / GETTER
 *--------------------------------------------------------------------------- */

/**
 *
 */
void stp_setPeriodStartRamp(uint16_t val)
{
	stpData.period.min = val;
}

void stp_setPeriodEndRamp(uint16_t val)
{
	stpData.period.max = val;
}

stpState_t stp_getState(void)
{
	return stpData.fsm.state;
}

/*------------------------------------------------------------------------------
 * ISR
 *--------------------------------------------------------------------------- */

/**
 * Interrupt for the stepper motor PIN
 *
 * It will ramp up the frequency and toggle the GPIO pin
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3) {

		if(stpData.fsm.state == STP_STATE_RAMP_UP) {
			if(stpData.period.val > stpData.period.max) {

				stpData.period.val -= stpData.period.step;
				__HAL_TIM_SET_AUTORELOAD(&htim3, stpData.period.val);
			}else {
				stpData.fsm.nxState = STP_STATE_RAMP_STABLE;
			}
		}

		stpData.steps.cnt++;

		/* Toggle the gpio pin */
		io_tglStpStep();
	}
}



