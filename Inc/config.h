/**
 * @file config.h
 * @author fl0mll
 * @date 2018/02/09
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Configuration settings interface
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * Longpress parameters
 */
#define APP_LONGPRESS_TIME_DEFAULT			(1000)
#define APP_LONGPRESS_TIME_MAX				(5000)
#define APP_LONGPRESS_TIME_MIN				(500)
#define APP_LONGPRESS_TIME_VADD				(0x1111)
#define APP_LONGPRESS_TIME_IDX				(0)

/**
 * Automatic power off parameters
 */
#define APP_POWER_OFF_DEFAULT				(30)
#define APP_POWER_OFF_MAX					(120)
#define APP_POWER_OFF_MIN					(1)
#define APP_POWER_OFF_VADD					(0x2222)
#define APP_POWER_OFF_IDX					(1)

/**
 * Number of steps from floor 0 to 1
 */
#define CFG_FLOOR_0_1_TICKS_DEFAULT			(30)
#define CFG_FLOOR_0_1_TICKS_MAX				(120)
#define CFG_FLOOR_0_1_TICKS_MIN				(1)
#define CFG_FLOOR_0_1_TICKS_VADD			(0x3333)
#define CFG_FLOOR_0_1_TICKS_IDX				(2)

/**
 * Number of steps from floor 1 to 2
 */
#define CFG_FLOOR_1_2_TICKS_DEFAULT			(30)
#define CFG_FLOOR_1_2_TICKS_MAX				(120)
#define CFG_FLOOR_1_2_TICKS_MIN				(1)
#define CFG_FLOOR_1_2_TICKS_VADD			(0x4444)
#define CFG_FLOOR_1_2_TICKS_IDX				(3)


extern uint16_t VirtAddVarTab[NumbOfVar];

#endif /* CONFIG_H_ */
