
/*
 * util_methods.h
 *
 *  Created on: Dec 2, 2018
 *      Author: gpi
 */

#ifndef VULKAN_UTILS_H_
#define VULKAN_UTILS_H_

#include <stdint.h>
#include <vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>


uint32_t new_GLFWwindow(GLFWwindow** ppGLFWwindow);

uint32_t new_Surface(VkSurfaceKHR *pSurface, GLFWwindow *window,
		const VkInstance instance);




#endif VULKAN_UTILS_H_
