#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>
#include "vulkan_methods.h"

int main(void) {

	puts("started successfully");

	uint32_t extensionCount = 1;
	const char*  extensionNames[] = { "VK_EXT_debug_utils" };
	uint32_t layerCount = 1;
	const char* layerNames[] = { "VK_LAYER_LUNARG_standard_validation" };

	VkInstance instance = createInstance(extensionCount, extensionNames, layerCount, layerNames);
	puts("created instance");
	VkDebugUtilsMessengerEXT callback = createDebugCallback(instance);
	puts("created debug callback");
	VkPhysicalDevice physicalDevice = createPhysicalDevice(instance);
	puts("created device");
	int32_t index = getDeviceQueueIndex(physicalDevice,VK_QUEUE_GRAPHICS_BIT);
	if(index == -1)
	{
		fprintf(stderr, "found no suitable queue on device, quitting\n");
	}
	VkDevice device = createLogicalDevice(physicalDevice,(uint32_t)index, 0, NULL, layerCount, layerNames);
	puts("created logical device");
	VkQueue graphicsQueue = createQueue(device, index);


	puts("cleaning up");
	destroyDevice(device);
	destroyDebugCallback(instance,callback);
	destroyInstance(instance);
	puts("done");
	return EXIT_SUCCESS;
}
