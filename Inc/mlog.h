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
#define mlog_debug(f_, ...) 			printf((f_), ##__VA_ARGS__)
#else
#define mlog_debug(f_, ...)				( (void)0 )
#endif

#if defined (MLOG_INFO)
#define mlog_info(f_, ...) 				printf((f_), ##__VA_ARGS__)
#else
#define mlog_info(f_, ...)				( (void)0 )
#endif

#if defined (MLOG_WARNING)
#define mlog_warning(f_, ...) 			printf((f_), ##__VA_ARGS__)
#else
#define mlog_info(f_, ...)				( (void)0 )
#endif

#endif /* MLOG_H_ */
