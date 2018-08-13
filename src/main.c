#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>
#include "vulkan_methods.h"

int main(void) {

	puts("started successfully");

	uint32_t extensionCount = 1;
	char* extensionNames[] = { "VK_EXT_debug_utils" };
	uint32_t layerCount = 1;
	char* layerNames[] = { "VK_LAYER_LUNARG_standard_validation" };

	VkInstance instance = createInstance(extensionCount, extensionNames, layerCount, layerNames);

	puts("created instance");

	VkDebugUtilsMessengerEXT callback = createDebugCallback(instance);

	puts("created debug callback");

	destroyDebugCallback(instance,callback);
	destroyInstance(instance);

	puts("done");

	return EXIT_SUCCESS;
}
