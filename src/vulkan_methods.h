/*
 * vulkan_methods.h
 *
 *  Created on: Aug 8, 2018
 *      Author: gpi
 */

#ifndef VULKAN_METHODS_H_
#define VULKAN_METHODS_H_

#include <stdbool.h>
#include <vulkan.h>

VkInstance createInstance(
		uint32_t				enabledExtensionCount,
		const char* const*		ppEnabledExtensionNames,
		uint32_t 				enabledLayerCount,
		const char* const* 		ppEnabledLayerNames);
void destroyInstance(VkInstance instance);
VkDebugUtilsMessengerEXT createDebugCallback(VkInstance instance);
void destroyDebugCallback(VkInstance instance, VkDebugUtilsMessengerEXT callback);

#endif /* VULKAN_METHODS_H_ */
