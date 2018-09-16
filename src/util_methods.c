/*
 * util_methods.c
 *
 *  Created on: Sep 15, 2018
 *      Author: gpi
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "util_methods.h"


void concatArray(size_t len1, void* src1, size_t len2, void* src2, size_t destlen, void* dest)
{
	memmove(dest, src1, len1);
	memmove(dest+len1, src2, len2);
}
