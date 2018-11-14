/*
 * glfw_methods.h
 *
 *  Created on: Sep 3, 2018
 *      Author: gpi
 */

#ifndef GLFW_METHODS_H_
#define GLFW_METHODS_H_

#include <vulkan.h>
#include <glfw3.h>

GLFWwindow *createGlfwWindow();
VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance instance);

#endif /* GLFW_METHODS_H_ */
