/**
 * @file version.c
 * @author fl0mll
 * @date 2018/01/01
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Version informations file
 */
#include <stdint.h>

#include "version.h"

/**
 * Helper macro to generate version number integer representation
 */
#define APP_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

/**
 * Integer representation of the application version number
 */
__attribute__((section(".myvars.VERSION_NUMBER"))) uint32_t VERSION_NUMBER = APP_VERSION_CHECK(APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_PATCH_VERSION);

/**
 * Placeholder for the application checksum
 *
 * The checksum will be calculated as a post build process
 */
__attribute__((section(".myvars.CRC"))) uint32_t CRC = 0xffffffff;

/**
 * TODO
 */
__attribute__((section(".myvars.BUILD_ID"))) uint16_t BUILD_ID = 0;
