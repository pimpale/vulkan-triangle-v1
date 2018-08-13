/*
 * error_handle.h
 *
 *  Created on: Aug 7, 2018
 *      Author: gpi
 */

#ifndef ERROR_HANDLE_H_
#define ERROR_HANDLE_H_

/**
 * Prints a null terminated string to stderr
 */
void printError(int errnum);
void printVulkanError(VkResult err);

void hardExit();

#endif /* ERROR_HANDLE_H_ */
