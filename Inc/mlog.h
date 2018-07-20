/**
 * @file log.h
 * @author fl0mll
 * @date 2018/07/13
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Logging macros
 */
#ifndef MLOG_H_
#define MLOG_H_

#include <stdio.h>

#define MLOG_ENABLED	(MLOG_DEBUG | MLOG_INFO | MLOG_WARNING)

#if defined(MLOG_DEBUG)
#define mDebug(f_, ...) 			printf((f_), ##__VA_ARGS__)
#else
#define mDebug(f_, ...)				( (void)0 )
#endif

#if defined (MLOG_INFO)
#define mInfo(f_, ...) 				printf((f_), ##__VA_ARGS__)
#else
#define mInfo(f_, ...)				( (void)0 )
#endif

#if defined (MLOG_WARNING)
#define mWarning(f_, ...) 			printf((f_), ##__VA_ARGS__)
#else
#define mWarning(f_, ...)				( (void)0 )
#endif

/**
 * @brief Write a fatal message to the io output and stop at infinite loop
 *
 * Fatal message are not able to disable because you should use this macro like a
 * function at important risk situations to stop the application
 */
#ifndef mFatal
#define mFatal(f_, ...) 			{printf((f_), ##__VA_ARGS__); while(1){}; }
#endif

#endif /* MLOG_H_ */
