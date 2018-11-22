#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>
#define GLFW_DEFINE_VULKAN
#include <glfw3.h>

#include "constants.h"
#include "error_methods.h"
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
			panic();
		}

		ppExtensionNames[0] = "VK_EXT_debug_utils";
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			ppExtensionNames[i + 1] = ppGlfwExtensionNames[i];
		}
	}

	/* Create instance */
	VkInstance instance;
	new_Instance(&instance, extensionCount, ppExtensionNames, layerCount,
			ppLayerNames);
	VkDebugUtilsMessengerEXT callback;
	new_DebugCallback(&callback, instance);

	/* Create instance */
	VkPhysicalDevice physicalDevice;
	getPhysicalDevice(&physicalDevice, instance);
	struct DeviceInfo deviceInfo;
	new_DeviceInfo(&deviceInfo, physicalDevice);

	/* Create window and surface */
	GLFWwindow *pWindow = createGlfwWindow();
	VkSurfaceKHR surface = createSurface(pWindow, instance);

	/* find queues on graphics device */
	uint32_t graphicsIndex;
	uint32_t computeIndex;
	uint32_t presentIndex;
	/*fail if our required indices are not present */
	if (getDeviceQueueIndex(&graphicsIndex, physicalDevice,
			VK_QUEUE_GRAPHICS_BIT) != VK_SUCCESS
			|| getDeviceQueueIndex(&computeIndex, physicalDevice,
			VK_QUEUE_COMPUTE_BIT) != VK_SUCCESS
			|| getPresentQueueIndex(&presentIndex, physicalDevice, surface
	) != VK_SUCCESS) {
		errLog(FATAL, "unable to acquire indices\n");
		panic();
	}

	/*create device */
	VkDevice device;
	new_Device(&device, deviceInfo, physicalDevice, graphicsIndex,
			deviceExtensionCount,
			ppDeviceExtensionNames, layerCount, ppLayerNames);

	/* get swap chain details */
	struct SwapChainInfo swapChainInfo = new_SwapChainInfo(physicalDevice,
			surface);

	/*Create swap chain */
	VkSwapchainKHR
	swapChain = new_SwapChain(VK_NULL_HANDLE,
			swapChainInfo,
			device,
			surface,
			(VkExtent2D ) { WINDOW_WIDTH, WINDOW_HEIGHT },
			deviceIndices);

	uint32_t swapChainImageCount = 0;
	VkImage* pSwapChainImages = NULL;
	VkImageView* pSwapChainImageViews = NULL;
	new_SwapChainImages(device, swapChain, &swapChainImageCount,
			&pSwapChainImages);
	new_SwapChainImageViews(device, swapChainInfo.preferredFormat.format,
			swapChainImageCount, pSwapChainImages, &pSwapChainImageViews);

	VkRenderPass renderPass = new_RenderPass(device,
			swapChainInfo.preferredFormat.format);

	VkPipeline graphicsPipeline;
	VkPipelineLayout graphicsPipelineLayout;

	uint32_t* fragShaderFileContents;
	uint32_t fragShaderFileLength;
	readShaderFile("assets/shaders/shader.frag.spv", &fragShaderFileLength,
			&fragShaderFileContents);
	VkShaderModule fragShaderModule = new_ShaderModule(device,
			fragShaderFileLength, fragShaderFileContents);

	uint32_t* vertShaderFileContents;
	uint32_t vertShaderFileLength;
	readShaderFile("assets/shaders/shader.vert.spv", &vertShaderFileLength,
			&vertShaderFileContents);
	VkShaderModule vertShaderModule = new_ShaderModule(device,
			vertShaderFileLength, vertShaderFileContents);



	/*wait till close*/
	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
	}

	/*cleanup*/
	delete_RenderPass(device, renderPass);
	delete_SwapChainImageViews(device, swapChainImageCount,
			pSwapChainImageViews);
	delete_SwapChainImages(pSwapChainImages);
	delete_SwapChain(device, swapChain);
	delete_SwapChainInfo(swapChainInfo);
	delete_DeviceIndices(deviceIndices);
	delete_Device(device);
	delete_DeviceInfo(deviceInfo);
	delete_DebugCallback(instance, callback);
	delete_Instance(instance);
	delete_InstanceInfo(instanceInfo);
	free(ppExtensionNames);
	glfwTerminate();
	return (EXIT_SUCCESS);
}
