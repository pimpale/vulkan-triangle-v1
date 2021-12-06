/* Copyright 2019 Govind Pimpale
 * util_methods.h
 *
 *  Created on: Sep 2, 2018
 *      Author: gpi
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stdint.h>
#include <stdio.h>

#include "errors.h"

uint64_t getLength(FILE *f);

void readShaderFile(const char *filename, uint32_t *length, uint32_t **code);

#endif /* SRC_UTILS_H_ */
