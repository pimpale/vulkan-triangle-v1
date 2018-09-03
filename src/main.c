#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <vulkan.h>
#define GLFW_DEFINE_VULKAN
#include <glfw3.h>

#include "error_methods.h"
#include "vulkan_methods.h"




int main(void) {
	glfwInit();

	//Initialize extensions to use
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	uint32_t extensionCount = 1 + glfwExtensionCount;
	const char** extensionNames = malloc(sizeof(char)*extensionCount);
	if(!extensionNames)
	{
		printError(errno);
		hardExit();
	}
	extensionNames[0] = "VK_EXT_debug_utils";
	for(uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		extensionNames[1+i] = glfwExtensionNames[i];
	}
	//Initialize layers to use
	uint32_t layerCount = 1;
	const char* layerNames[] = { "VK_LAYER_LUNARG_standard_validation" };


	//Create instance
	VkInstance instance = createInstance(extensionCount, extensionNames, layerCount, layerNames);
	VkDebugUtilsMessengerEXT callback = createDebugCallback(instance);
	VkPhysicalDevice physicalDevice = createPhysicalDevice(instance);
	int32_t index = getDeviceQueueIndex(physicalDevice,VK_QUEUE_GRAPHICS_BIT);
	if(index == -1)
	{
		fprintf(stderr, "found no suitable queue on device, quitting\n");
	}
	VkDevice device = createLogicalDevice(physicalDevice,(uint32_t)index, 0, NULL, layerCount, layerNames);
	VkQueue graphicsQueue = createQueue(device, index);




	destroyDevice(device);
	destroyDebugCallback(instance,callback);
	destroyInstance(instance);
	return EXIT_SUCCESS;
}
