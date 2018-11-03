#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>
#define GLFW_DEFINE_VULKAN
#include <glfw3.h>

#include "constants.h"
#include "error_methods.h"
#include "glfw_methods.h"
#include "util_methods.h"
#include "vulkan_methods.h"

int main(void) {
	glfwInit();

	/* Extensions, Layers, and Device Extensions declared (some initialized) */
	uint32_t extensionCount;
	const char **ppExtensionNames;
	uint32_t layerCount = 1;
	const char *ppLayerNames[] = { "VK_LAYER_LUNARG_standard_validation" };
	uint32_t deviceExtensionCount = 1;
	const char *ppDeviceExtensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	/* set other uninitialized stuff */
	{
		/* define our own extensions */
		/* get glfw extensions to use */
		uint32_t glfwExtensionCount = 0;
		const char **ppGlfwExtensionNames = glfwGetRequiredInstanceExtensions(
				&glfwExtensionCount);
		extensionCount = 1 + glfwExtensionCount;

		ppExtensionNames = malloc(sizeof(char *) * extensionCount);

		if (!ppExtensionNames) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			hardExit();
		}

		ppExtensionNames[0] = "VK_EXT_debug_utils";
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			ppExtensionNames[i + 1] = ppGlfwExtensionNames[i];
		}
	}



	/*get instance info */
	struct InstanceInfo instanceInfo = new_InstanceInfo();

	/* Create instance */
	VkInstance instance = new_Instance(instanceInfo, extensionCount,
			ppExtensionNames, layerCount, ppLayerNames);
	VkDebugUtilsMessengerEXT callback = new_DebugCallback(instance);

	VkPhysicalDevice physicalDevice = getPhysicalDevice(instance);
	struct DeviceInfo deviceInfo = new_DeviceInfo(physicalDevice);


	/* Create window and surface */
	GLFWwindow *pWindow = createGlfwWindow();
	VkSurfaceKHR surface = createSurface(pWindow, instance);

	/* find queues on graphics device */
	struct DeviceIndices deviceIndices = getDeviceIndices(physicalDevice,
			surface);
	/*fail if our required indices are not present */
	if (!(deviceIndices.hasGraphics && deviceIndices.hasCompute
			&& deviceIndices.hasPresent)) {
		errLog(FATAL, "unable to acquire indices\n");
	}
	/*create device */
	VkDevice device = new_LogicalDevice(deviceInfo, physicalDevice,
			deviceIndices.graphicsIndex, deviceExtensionCount,
			ppDeviceExtensionNames, layerCount, ppLayerNames);

	/* create queues */
	VkQueue graphicsQueue = getQueue(device, deviceIndices.graphicsIndex);
	VkQueue presentQueue = getQueue(device, deviceIndices.presentIndex);

	/*Create swap chain */
	VkSwapchainKHR
	swapChain = new_SwapChain(VK_NULL_HANDLE,
			device,
			physicalDevice,
			surface,
			(VkExtent2D ) { WINDOW_WIDTH, WINDOW_HEIGHT },
			deviceIndices);




	/*wait till close*/
	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
	}

	/*cleanup*/
	delete_SwapChain(device, swapChain);
	delete_Device(device);
	delete_DeviceInfo(deviceInfo);
	delete_DebugCallback(instance, callback);
	delete_Instance(instance);
	delete_InstanceInfo(instanceInfo);
	free(ppExtensionNames);
	glfwTerminate();
	return (EXIT_SUCCESS);
}
