/**
 * @file io.h
 * @author fl0mll
 * @date 2018/07/22
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief I/O helper macros
 */

#ifndef IO_H_
#define IO_H_

#include "stm32f1xx_hal.h"

/*
 * Outputs
 */
#define io_setStpEnable()       HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_RESET);
#define io_clrStpEnable()       HAL_GPIO_WritePin(MTR_nENABLE_GPIO_Port, MTR_nENABLE_Pin, GPIO_PIN_SET);

#define io_setStpReset()        HAL_GPIO_WritePin(MTR_nRESET_GPIO_Port, MTR_nRESET_Pin, GPIO_PIN_RESET);
#define io_clrStpReset()        HAL_GPIO_WritePin(MTR_nRESET_GPIO_Port, MTR_nRESET_Pin, GPIO_PIN_SET);

#define io_setStpSleep()        HAL_GPIO_WritePin(MTR_nSLEEP_GPIO_Port, MTR_nSLEEP_Pin, GPIO_PIN_RESET);
#define io_clrStpSleep()        HAL_GPIO_WritePin(MTR_nSLEEP_GPIO_Port, MTR_nSLEEP_Pin, GPIO_PIN_SET);

#define io_tglStpStep()         HAL_GPIO_TogglePin(MTR_STEP_GPIO_Port, MTR_STEP_Pin);

#define io_setMtrMs1()          HAL_GPIO_WritePin(MTR_MS1_GPIO_Port, MTR_MS1_Pin, GPIO_PIN_SET);
#define io_clrMtrMs1()          HAL_GPIO_WritePin(MTR_MS1_GPIO_Port, MTR_MS1_Pin, GPIO_PIN_RESET);

#define io_setMtrMs2()          HAL_GPIO_WritePin(MTR_MS2_GPIO_Port, MTR_MS2_Pin, GPIO_PIN_SET);
#define io_clrMtrMs2()          HAL_GPIO_WritePin(MTR_MS2_GPIO_Port, MTR_MS2_Pin, GPIO_PIN_RESET);

#define io_setMtrMs3()          HAL_GPIO_WritePin(MTR_MS3_GPIO_Port, MTR_MS3_Pin, GPIO_PIN_SET);
#define io_clrMtrMs3()          HAL_GPIO_WritePin(MTR_MS3_GPIO_Port, MTR_MS3_Pin, GPIO_PIN_RESET);

#define io_setLd1()             HAL_GPIO_WritePin(LD1_OUT_GPIO_Port, LD1_OUT_Pin, GPIO_PIN_RESET);
#define io_clrLd1()             HAL_GPIO_WritePin(LD1_OUT_GPIO_Port, LD1_OUT_Pin, GPIO_PIN_SET);
#define io_tglLd1()             HAL_GPIO_TogglePin(LD1_OUT_GPIO_Port, LD1_OUT_Pin);

/*
 * Inputs
 */
#define io_isSw1()              (HAL_GPIO_ReadPin(SW1_IN_GPIO_Port, SW1_IN_Pin) == GPIO_PIN_RESET)
#define io_isSw2()              (HAL_GPIO_ReadPin(SW2_IN_GPIO_Port, SW2_IN_Pin) == GPIO_PIN_RESET)


#endif /* IO_H_ */
