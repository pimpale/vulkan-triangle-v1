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

#include "constants.h"
#include "errors.h"
#include "utils.h"

static long getLength(FILE* f) {
	// get current position in file
	long currentpos = ftell(f);

	// go to end and find length
	fseek(f, 0, SEEK_END);
	long  size = ftell(f);

	// restore current position
	fseek(f, currentpos, SEEK_SET);

	// return size
	return (size);
}

/**
 * Mallocs
 */
void readShaderFile(const char* filename, uint32_t* length, uint32_t** code) {
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		errLog(FATAL, "could not read file\n");
		panic();
	}

	long rawfilesize = getLength(fp);

  if(rawfilesize < 0 || rawfilesize > UINT32_MAX) {
      errLog(FATAL, "File size is invalid.");
      fclose(fp);
      panic();
  }

  uint32_t filesize = (uint32_t)rawfilesize;

  // align the filesize to a multiple of 4
	uint32_t filesizepadded = (filesize + 3) & ~0x3u;

	uint32_t *data = malloc(filesizepadded);
	if (!data) {
		errLog(FATAL, "Could not allocate space for shader file: %s\n", strerror(errno));
		fclose(fp);
		panic();
	}

	size_t bytes_read = fread(data, filesize, 1, fp);

  // handle if we were unable to read the shader file
	if(bytes_read < filesize) {
		errLog(FATAL, "Could not read shader file: %s\n", strerror(errno));
		fclose(fp);
		panic();
	}

	fclose(fp);

	// pad the rest of the bytes
	for (uint32_t i = filesize; i < filesizepadded; i++) {
		data[i] = 0;
	}

	// return data
	*length = filesizepadded;
	*code = data;
	return;
}
