/*
 * util_methods.h
 *
 *  Created on: Sep 2, 2018
 *      Author: gpi
 */

#ifndef UTIL_METHODS_H_
#define UTIL_METHODS_H_

void concatArray(void *src1, uint32_t len1, void *src2, uint32_t len2,
                 void *dest, uint32_t destlen);

void findMatchingStrings(const char *const *ppData, uint32_t dataLen,
                         const char *const *ppQuery, uint32_t queryLen,
                         char **ppResult, uint32_t resultLen,
                         uint32_t *pMatches);

void readShaderFile(char* filename, uint32_t* length, uint32_t** code);

#endif /* UTIL_METHODS_H_ */
