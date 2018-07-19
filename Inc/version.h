/**
 * @file version.h
 * @author fl0mll
 * @date 2017/12/30
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Version informations file
 */
#ifndef VERSION_H_
#define VERSION_H_

#define APP_MAJOR_VERSION 	(1)
#define APP_MINOR_VERSION 	(0)
#define APP_PATCH_VERSION 	(0)

#define APP_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define APP_VERSION      APP_VERSION_CHECK(APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_PATCH_VERSION)

/* Application Information Attributes */
#define APPINFO_VERSION_ATTR			__attribute__((section(".appinfo.VERSION_NUMBER")))
#define APPINFO_CRC_ATTR				__attribute__((section(".appinfo.CRC")))
#define APPINFO_BUILDID_ATTR			__attribute__((section(".appinfo.BUILD_ID")))
#define APPINFO_BUILD_DATETIME_ATTR		__attribute__((section(".appinfo.BUILD_DATETIME")))
#define APPINFO_BUILD_GITHASH_ATTR		__attribute__((section(".appinfo.BUILD_GITHASH")))

typedef struct appInfo_s {
	/**
	 * Structure version information
	 *
	 * The first version number
	 *
	 * Offset: 0
	 */
	uint8_t sectionVersion;

	struct {
		uint8_t major;
		uint8_t minor;
		uint8_t patch;
		uint8_t ext;
		uint8_t num;
	} version;

	uint16_t buildnum;

	struct {
		uint16_t year;
		uint8_t month;
		uint8_t day;
	} buildDate;

	uint32_t validEntry;
	uint32_t product;
	uint32_t customer;

	struct {
		uint8_t info;
		uint32_t sum;
	} checksum;

} appInfo_t;

#endif /* VERSION_H_ */
