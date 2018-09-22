/*
 * util_methods.c
 *
 *  Created on: Sep 15, 2018
 *      Author: gpi
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#include "util_methods.h"


/* Preconditions: src1 must not null, and it must have its size in bytes in len1. 
 * src2 must not be null, and its size in bytes must be stored in len2 
 * dest must not be null, and its size in bytes must be stored in destlen
 *
 * Output: src1 and src2 are guaranteed not to be modified
 * dest will contain all bytes in src1 followed by all bytes in src2
 *
 * Returns: void
 */
void concatArray(void* src1, size_t len1, void* src2, size_t len2, void* dest, size_t destlen)
{
	assert(src1 != NULL && src2 != NULL && dest != NULL);
	memmove(dest, src1, len1);
	memmove(dest+len1, src2, len2);
}
/* Preconditions: neither data nor query may be null, and 
 *
 */
void findMatchingStrings(const char* const* data, size_t datalen, 
		const char* const* query, size_t querylen,
		char** result, size_t resultlen,
		size_t* matchnum)
{
	assert(query != NULL && data != NULL);
	
	//to fill the results value out
	bool fillResults = result != NULL;
	
	*matchnum = 0;

	for(size_t i = 0; i < querylen; i++)
	{
		for(size_t a = 0; a < datalen; a++)
		{
			if(strcmp(query[i], data[a]) == 0)
			{
				if(fillResults && *matchnum < resultlen)
				{
					strcpy(result[*matchnum],query[i]);
					(*matchnum)++;
					break;
				}
			}		
		}
	}
}

