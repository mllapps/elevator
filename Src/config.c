/**
 * @file config.c
 * @author fl0mll
 * @date 2018/02/09
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Configuration settings implementation
 */
#include "config.h"

/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[] = {
		CFG_LONGPRESS_TIME_VADDR,
		CFG_POWER_OFF_VADDR,
		CFG_FLOOR_0_1_TICKS_VADDR,
		CFG_FLOOR_1_2_TICKS_VADDR,
		CFG_TIMEOUT_FLOOR2_ARRIVE_VADDR,
		0x0000	/* End of the list */
};
