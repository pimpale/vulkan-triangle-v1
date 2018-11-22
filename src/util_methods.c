/*
 * util_methods.c
 *
 *  Created on: Sep 15, 2018
 *      Author: gpi
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "error_methods.h"
#include "constants.h"
#include "util_methods.h"

/* Preconditions: neither ppData nor ppQuery may be null.
 * dataLen must be the number of entries in ppData
 * queryLen must be the number of entries in ppQuery
 * Each char* in any of the two arrays must be null terminated.
 * pMatches must not be NULL.
 * ppResult, however, may be null. If so, then its value as well as
 * the value of resultLen will be ignored.
 * However, if ppResult is not null, then resultLen must
 * contain the number of entries in ppResult.
 *
 * Output: pMatches will always be set to the number of strings
 * in ppQuery that also appear in ppData.
 * If ppResult is not null, then each string that occurs in
 * both ppData and ppQuery will be placed in ppResult in no
 * guaranteed order.
 *
 * If ppResult has more room, then the entries which have indices
 * that are greater than the number of matches will not be touched.
 *
 * Returns: void
 *
 * TODO prevent overflows
 */
void findMatchingStrings(const char *const *ppData, uint32_t dataLen,
		const char * const *ppQuery, uint32_t queryLen, char **ppResult,
		uint32_t resultLen, uint32_t *pMatches) {

	/*to fill the ppResults value out*/
	int fillResults = ppResult != NULL;

	*pMatches = 0;

	for (uint32_t i = 0; i < queryLen; i++) {
		for (uint32_t a = 0; a < dataLen; a++) {
			if (strcmp(ppQuery[i], ppData[a]) == 0) {
				if (fillResults && *pMatches < resultLen) {
					strcpy(ppResult[*pMatches], ppQuery[i]);
				}
				(*pMatches)++;
				break;
			}
		}
	}
}

uint64_t getLength(FILE* f) {
	uint64_t currentpos = ftell(f);
	fseek(f, 0, SEEK_END);
	uint64_t size = ftell(f);
	fseek(f, currentpos, SEEK_SET);
	return (size);
}
/**
 * Mallocs
 */
void readShaderFile(char* filename, uint32_t* length, uint32_t** code) {
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		errLog(FATAL, "could not read file\n");
		panic();
	}

	uint64_t filesize = getLength(fp);
	uint64_t filesizepadded =
			filesize % 4 == 0 ? filesize * 4 : (filesize + 1) * 4;

	char *str = malloc(filesizepadded);
	if (!str) {
		errLog(FATAL, "Could not read shader file: %s\n", strerror(errno));
		fclose(fp);
		panic();
	}

	fread(str, filesize, sizeof(char), fp);
	fclose(fp);

	/*pad data*/
	for (int i = filesize; i < filesizepadded; i++) {
		str[i] = 0;
	}

	/*set up*/
	*length = filesizepadded;
	*code = (uint32_t*) str;
}
