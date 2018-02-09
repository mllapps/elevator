/**
 * @file app.h
 * @author fl0mll
 * @date 2017/12/27
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Application interface
 */

#ifndef APP_H_
#define APP_H_

#include "stm32f1xx_hal.h"

/**
 * Avoid "unused parameter" warnings
 */
#define UNUSED(x) (void)x;

/**
 * Fixed interval to trigger the button
 */
#define APP_BUTTON_TRIGGER_INTERVAL			(10)

void app_init();
void app_handler();

#endif /* APP_H_ */
