/*
 * util_methods.c
 *
 *  Created on: Sep 15, 2018
 *      Author: gpi
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "constants.h"
#include "errors.h"

#include "utils.h"


uint64_t getLength(FILE *f) {
  /* TODO what if the file is modified as we read it? */
  int64_t currentpos = ftell(f);
  fseek(f, 0, SEEK_END);
  int64_t size = ftell(f);
  fseek(f, currentpos, SEEK_SET);
  if (size < 0) {
    LOG_ERROR(ERR_LEVEL_ERROR, "invalid file size");
    return (0);
  }
  return ((uint64_t)size);
}

/**
 * Mallocs
 */
void readShaderFile(const char *filename, uint32_t *length, uint32_t **code) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    LOG_ERROR(ERR_LEVEL_FATAL, "could not read file");
    PANIC();
  }
  /* We can coerce to a 32 bit, because no realistic files will be
   * greater than 2 GB */
  uint32_t filesize = (uint32_t)getLength(fp);
  // pad to a multiple of 4
  uint32_t filesizepadded = (filesize + 3) & ~0x03u;

  char *str = malloc(filesizepadded);
  if (!str) {
    LOG_ERROR_ARGS(ERR_LEVEL_FATAL, "could not read shader file: %s",
                   strerror(errno));
    fclose(fp);
    PANIC();
  }

  fread(str, filesize, sizeof(char), fp);
  fclose(fp);

  /*pad data*/
  for (uint32_t i = filesize; i < filesizepadded; i++) {
    str[i] = 0;
  }

  /*cast data t array of uints*/
  *length = filesizepadded;
  *code = (uint32_t *)((void *)str);
  return;
}
