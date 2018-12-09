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

#define ESUCCESS 0
#define ENOTSUPPORTED 1
#define EUNSAFE 2
#define EBADARGS 3
#define EOPFAIL 4
#define EOUTOFDATE 5

void errLog(int level, const char* message, ...);


void panic();

#endif /* ERRORS_H_ */
