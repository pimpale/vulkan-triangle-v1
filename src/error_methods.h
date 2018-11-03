/*
 * error_handle.h
 *
 *  Created on: Aug 7, 2018
 *      Author: gpi
 */

#ifndef ERROR_METHODS_H_
#define ERROR_METHODS_H_

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4
#define FATAL 5
#define UNKNOWN 6

#define DEBUG_MSG "DEBUG"
#define INFO_MSG "INFO"
#define WARN_MSG "WARN"
#define ERROR_MSG "ERROR"
#define FATAL_MSG "FATAL"
#define UNKNOWN_MSG "UNKNOWN_SEVERITY"

/**
 * Prints a null terminated string to stderr
 */
void printError(int errnum);

void errLog(int level, const char* message, ...);

void hardExit();

#endif /* ERROR_METHODS_H_ */
