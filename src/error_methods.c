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
#include <vulkan/vulkan.h>

#include "constants.h"

#include "error_methods.h"



void printVulkanError(VkResult error) {
	char buf[MAX_PRINT_LENGTH];
	if (error != VK_SUCCESS) {
		char* buf = snprintf(buf, MAX_PRINT_LENGTH, "Vulkan error: %i\n",
				(int) error);
		errLog(ERROR, buf);
	}
}

void printError(int errnum) {
	errLog(ERROR, strerror(errnum));
}

void errLog(int level, const char* msg) {
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

	fprintf(out, "%s: %s: %s", APPNAME, errmsg, msg);
}

void hardExit() { exit(EXIT_FAILURE); }
