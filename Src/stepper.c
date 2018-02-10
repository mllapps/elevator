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

typedef enum {
	STP_STATE_IDLE					= 0,

	STP_STATE_RAMP_START			= 1,
	STP_STATE_RAMP_UP				= 2,
	STP_STATE_RAMP_STABLE			= 3,
	STP_STATE_ARRIVED				= 4,

	STP_STATE_FAULT					= 10,
	STP_STATE_FAULT_INVALID_DIR		= 11
} stpState_t;

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
		uint32_t max;
		uint32_t steps;
	} period;

} stpData_t;

static stpData_t stpData;

void stp_setDecayMode(void);

void stp_init(void)
{
	stpData.fsm.state =
	stpData.fsm.nxState = STP_STATE_IDLE;

	stpData.steps.cnt =
	stpData.steps.target = 0;

	stpData.cmd.active =
	stpData.cmd.nxt = STP_CMD_NONE;

	stpData.period.val = 0;
	stpData.period.max = 0;
	stpData.period.steps = 10;

	stp_setDecayMode();
}

void stp_deinit(void)
{
	/* Disable the timer */
	HAL_TIM_Base_Stop(&htim3);

	/* Disable the timer interrupt */
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);

	/* Disable the driver */
	HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_SET);

	/* Enable sleep mode */
	HAL_GPIO_WritePin(MTR_nSLEEP_GPIO_Port, MTR_nSLEEP_Pin, GPIO_PIN_RESET);
}

void stp_handler(void)
{
	switch(stpData.fsm.state){
	case STP_STATE_ARRIVED:
	case STP_STATE_IDLE:
		/* Disable the timer */
		HAL_TIM_Base_Stop(&htim3);

		/* Disable the driver */
		HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_SET);

		/* Enable sleep mode */
		HAL_GPIO_WritePin(MTR_nSLEEP_GPIO_Port, MTR_nSLEEP_Pin, GPIO_PIN_RESET);

		if(stpData.cmd.active == STP_CMD_DRIVE_UP || stpData.cmd.active == STP_CMD_DRIVE_DOWN) {
			stpData.fsm.nxState = STP_STATE_RAMP_UP;
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

		/* Enable the motor driver */
		HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_RESET);

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
			stpData.fsm.state = STP_STATE_IDLE;
		}else if(stpData.steps.cnt >= stpData.steps.target)
		{
			stpData.fsm.state = STP_STATE_ARRIVED;
		}
		break;
	case STP_STATE_RAMP_STABLE:

		/* Transitions */
		if(stpData.cmd.active == STP_CMD_STOP)
		{
			stpData.fsm.state = STP_STATE_IDLE;
		}else if(stpData.steps.cnt >= stpData.steps.target)
		{
			stpData.fsm.state = STP_STATE_ARRIVED;
		}
		break;

	case STP_STATE_FAULT:
		/* Disable the driver */
		HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_SET);
		break;
	case STP_STATE_FAULT_INVALID_DIR:
		/* Disable the driver */
		HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_SET);
		break;
	default:
		break;
	}
}

void stp_setDecayMode(void)
{
	/* Setup the decay mode */
	HAL_GPIO_WritePin(MTR_MS1_GPIO_Port, MTR_MS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MTR_MS2_GPIO_Port, MTR_MS2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MTR_MS3_GPIO_Port, MTR_MS3_Pin, GPIO_PIN_SET);
}

void stp_requ(stpCmd_t cmd)
{
	stpData.cmd.nxt = cmd;
}

/**
 *
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3) {

		if(stpData.fsm.state == STP_STATE_RAMP_UP) {
			if(stpData.period.val > stpData.period.max) {

				stpData.period.val -= stpData.period.steps;
				__HAL_TIM_SET_AUTORELOAD(&htim3, stpData.period.val);
			}else {
				stpData.fsm.nxState = STP_STATE_RAMP_STABLE;
			}
		}
	}
}



