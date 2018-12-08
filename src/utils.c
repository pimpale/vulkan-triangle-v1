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

#include <vulkan.h>
#define GLFW_DEFINE_VULKAN
#include <glfw3.h>

#include "constants.h"
#include "errors.h"
#include "utils.h"

GLFWwindow *createGlfwWindow() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	return (glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", NULL, NULL));
}

VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance instance) {
	VkSurfaceKHR surface = { 0 };
	VkResult res = glfwCreateWindowSurface(instance, window, NULL, &surface);
	if (res != VK_SUCCESS) {
		fprintf(stderr, "failed to create surface, quitting\n");
		panic();
	}
	return (surface);
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

	uint32_t filesize = getLength(fp);
	uint32_t filesizepadded = (
			filesize % 4 == 0 ? filesize * 4 : (filesize + 1) * 4) / 4;

	char *str = malloc(filesizepadded);
	if (!str) {
		errLog(FATAL, "Could not read shader file: %s\n", strerror(errno));
		fclose(fp);
		panic();
		return;
	}

	fread(str, filesize, sizeof(char), fp);
	fclose(fp);

	/*pad data*/
	for (uint32_t i = filesize; i < filesizepadded; i++) {
		str[i] = 0;
	}

	/*set up*/
	*length = filesizepadded;
	*code = (uint32_t*) str;
	return;
}
