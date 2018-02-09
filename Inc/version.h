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


#endif /* VERSION_H_ */
