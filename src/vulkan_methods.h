/*
 * vulkan_methods.h
 *
 *  Created on: Aug 8, 2018
 *      Author: gpi
 */

#ifndef VULKAN_METHODS_H_
#define VULKAN_METHODS_H_

#include <stdbool.h>
#include <stdint.h>
#include <vulkan.h>

#define NULL_INDEX -1


/**
 * This struct contains data for a physical
 * device so that it doesn't have to continuously be fetched
 */
struct DeviceInfo {
	//all arrays will be malloc'ed and freed
	//at getDeviceInfo and destroyDeviceInfo, respectively
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	uint32_t extensionCount;
	char** ppExtensionNames;
	uint32_t layerCount;
	char** ppLayerNames;
};

struct InstanceInfo {
	uint32_t extensionCount;
	char** ppExtensionNames;
	uint32_t layerCount;
	char** ppLayerNames;
};

/**
 * Queue family indices
 */
struct DeviceIndices {
	uint32_t computeIndex;
	uint32_t graphicsIndex;
	uint32_t presentIndex;
	bool hasCompute;
	bool hasGraphics;
	bool hasPresent;
};

VkInstance createInstance(struct InstanceInfo instanceInfo,
		uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames);

void destroyInstance(VkInstance instance);

VkDebugUtilsMessengerEXT createDebugCallback(VkInstance instance);

void destroyDebugCallback(VkInstance instance,
		VkDebugUtilsMessengerEXT callback);

VkPhysicalDevice createPhysicalDevice(VkInstance instance);

VkDevice createLogicalDevice(struct DeviceInfo deviceInfo,
		VkPhysicalDevice physicalDevice,
		uint32_t deviceQueueIndex,
		uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames);

void destroyDevice(VkDevice device);

int32_t getDeviceQueueIndex(VkPhysicalDevice device, VkQueueFlags bit);
int32_t getPresentQueueIndex(VkPhysicalDevice device, VkSurfaceKHR surface);

VkQueue createQueue(VkDevice device, uint32_t deviceQueueIndex);

#endif /* VULKAN_METHODS_H_ */
