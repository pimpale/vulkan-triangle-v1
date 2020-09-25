/*
 * error_handle.h
 *
 *  Created on: Aug 7, 2018
 *      Author: gpi
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4
#define FATAL 5
#define UNKNOWN 6

#define DEBUG_MSG "debug"
#define INFO_MSG "info"
#define WARN_MSG "warning"
#define ERROR_MSG "error"
#define FATAL_MSG "fatal"
#define UNKNOWN_MSG "unknown severity"

#define UNUSED(x) (void)(x)

#define ERR_NONE 0
#define ERR_NOTSUPPORTED 1
#define ERR_UNSAFE 2
#define ERR_BADARGS 3
#define ERR_UNKNOWN 4
#define ERR_OUTOFDATE 5

void errLog(int level, const char* message, ...);


void panic();

#endif /* ERRORS_H_ */
