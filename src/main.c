#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>
#define GLFW_DEFINE_VULKAN
#include <glfw3.h>

#include "error_methods.h"
#include "glfw_methods.h"
#include "util_methods.h"
#include "vulkan_methods.h"

int main(void) {
	glfwInit();

	//Extensions, Layers, and Device Extensions declared (some initialized
	uint32_t extensionCount;
	const char **ppExtensionNames;
	uint32_t layerCount = 1;
	const char *ppLayerNames[] = { "VK_LAYER_LUNARG_standard_validation" };
	uint32_t deviceExtensionCount = 1;
	const char *ppDeviceExtensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	//set other uninitialized stuff
	{
		// define our own extensions
		// get glfw extensions to use
		uint32_t glfwExtensionCount = 0;
		const char **ppGlfwExtensionNames = glfwGetRequiredInstanceExtensions(
				&glfwExtensionCount);
		extensionCount = 1 + glfwExtensionCount;

		ppExtensionNames = malloc(sizeof(char *) * extensionCount);

		if (!ppExtensionNames) {
			printError(errno);
			hardExit();
		}

		ppExtensionNames[0] = "VK_EXT_debug_utils";
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			ppExtensionNames[i + 1] = ppGlfwExtensionNames[i];
		}
	}



	//get instance info
	struct InstanceInfo instanceInfo = getInstanceInfo();

	// Create instance
	VkInstance instance = createInstance(instanceInfo, extensionCount,
			ppExtensionNames, layerCount, ppLayerNames);
	VkDebugUtilsMessengerEXT callback = createDebugCallback(instance);

	VkPhysicalDevice physicalDevice = createPhysicalDevice(instance);
	struct DeviceInfo deviceInfo = getDeviceInfo(physicalDevice);


	// Create window and surface
	GLFWwindow *pWindow = createGlfwWindow();
	VkSurfaceKHR surface = createSurface(pWindow, instance);

	// find queue on graphics device
	int32_t graphicsQueueIndex = getDeviceQueueIndex(physicalDevice,
			VK_QUEUE_GRAPHICS_BIT);
	int32_t presentQueueIndex = getPresentQueueIndex(physicalDevice, surface);
	if (graphicsQueueIndex == -1 || presentQueueIndex == -1) {
		fprintf(stderr, "found no suitable queue on device, quitting\n");
		hardExit();
	}
	VkDevice device = createLogicalDevice(deviceInfo, physicalDevice,
			(uint32_t) graphicsQueueIndex, deviceExtensionCount,
			ppDeviceExtensionNames, layerCount, ppLayerNames);

	// create queues
	VkQueue graphicsQueue = createQueue(device, (uint32_t) graphicsQueueIndex);
	VkQueue presentQueue = createQueue(device, (uint32_t) presentQueueIndex);

	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
	}

	puts("quitting");

	destroyDevice(device);
	destroyDebugCallback(instance, callback);
	destroyInstance(instance);
	free(ppExtensionNames);
	glfwTerminate();
	return EXIT_SUCCESS;
}
