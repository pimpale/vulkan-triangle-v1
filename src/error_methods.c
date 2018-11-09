/*
 * error_handle.c
 *
 *  Created on: Aug 7, 2018
 *      Author: gpi
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "constants.h"
#include "error_methods.h"

void errLog(int level, const char* message, ...) {
	char* errmsg;
	FILE* out;

	switch (level) {
	case DEBUG: {
		errmsg = DEBUG_MSG;
		out = stdout;
		break;
	}
	case INFO: {
		errmsg = INFO_MSG;
		out = stdout;
		break;
	}
	case WARN: {
		errmsg = WARN_MSG;
		out = stderr;
		break;
	}
	case ERROR: {
		errmsg = ERROR_MSG;
		out = stderr;
		break;
	}
	case FATAL: {
		errmsg = FATAL_MSG;
		out = stderr;
		break;
	}
	default: {
		errmsg = INFO_MSG;
		out = stdout;
		break;
	}
	}

	char message_formatted[MAX_PRINT_LENGTH];
	va_list args;
	va_start(args, message);
	vsnprintf(message_formatted, MAX_PRINT_LENGTH, message, args);
	va_end(args);

	fprintf(out, "%s: %s: %s", APPNAME, errmsg, message_formatted);
}

void panic() { exit(EXIT_FAILURE); }
