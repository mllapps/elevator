/**
 * @file stepper.h
 * @author fl0mll
 * @date 2018/02/09
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Stepper motor control interface
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "stm32f1xx_hal.h"

typedef enum stpCmd_e {
	STP_CMD_NONE		= 0,
	STP_CMD_STOP		= 1,
	STP_CMD_ARRIVED		= 2,
	STP_CMD_DRIVE_UP	= 3,
	STP_CMD_DRIVE_DOWN	= 4
} stpCmd_t;

void stp_init(void);
void stp_deinit(void);
void stp_handler(void);

void stp_requ(stpCmd_t cmd);

#endif /* STEPPER_H_ */
