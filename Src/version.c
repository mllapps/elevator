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
#include "versioncc.h"

/**
 * Helper macro to generate version number integer representation
 */
#define APP_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

/**
 * Integer representation of the application version number
 */
const uint32_t VERSION_NUMBER APPINFO_VERSION_ATTR = APP_VERSION_CHECK(APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_PATCH_VERSION);

/**
 * Placeholder for the application checksum
 *
 * The checksum will be calculated as a post build process
 */
const uint32_t CRC APPINFO_CRC_ATTR = 0xFFFFFFFF;

/**
 * Build number
 */
//const uint16_t BUILD_ID APPINFO_BUILDID_ATTR = 0;

//const char BUILD_DATETIME[] APPINFO_BUILD_DATETIME_ATTR = __TIMESTAMP_ISO__;

const char BUILD_GITCHECKSUM[] APPINFO_BUILD_GITHASH_ATTR = "";


appInfo_t appInfo = {
        .sectionVersion     = 1,

        .version.major      = APP_MAJOR_VERSION,
        .version.minor      = APP_MINOR_VERSION,
        .version.patch      = APP_PATCH_VERSION,
        .version.ext        = APP_EXT_VERSION,
        .version.buildnum   = __BUILD_NUMBER__,

        .buildDate.year     = __BUILD_DATEYEAR__,
        .buildDate.month    = __BUILD_DATEMONTH__,
        .buildDate.day      = __BUILD_DATEDAY__,

        .validEntry         = APP_VALID_ENTRY,

        .product            = APP_PRODUCT_ID,
        .customer           = APP_CUSTOMER_ID,
};
