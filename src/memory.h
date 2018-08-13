/*
 * memory.h
 *
 *  Created on: Aug 7, 2018
 *      Author: gpi
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdlib.h>
#include <errno.h>
#include "constants.h"
#include "error_handle.h"

/**
 * If malloc fails, it terminates the program. Only use for critical allocs
 */
void* criticalMalloc(size_t size)
{
	void* mem = malloc(size);
	if(mem == NULL)
	{
		printError(errno);
		hardExit();
	}
	return mem;
}

/**
 * Frees and sets to null automatically
 */
#define xfree(ptr) do {free(ptr);ptr=NULL;} while(0)

#endif /* MEMORY_H_ */
