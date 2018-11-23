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

	/* get physical device */
	VkPhysicalDevice physicalDevice;
	getPhysicalDevice(&physicalDevice, instance);

	/* Get device info */
	struct DeviceInfo deviceInfo;
	new_DeviceInfo(&deviceInfo, physicalDevice);

	/* Create window and surface */
	GLFWwindow *pWindow = createGlfwWindow();
	VkSurfaceKHR surface = createSurface(pWindow, instance);

	/* find queues on graphics device */
	uint32_t graphicsIndex;
	uint32_t computeIndex;
	uint32_t presentIndex;
	{
		uint32_t ret1 = getDeviceQueueIndex(&graphicsIndex, physicalDevice,
				VK_QUEUE_GRAPHICS_BIT);
		uint32_t ret2 = getDeviceQueueIndex(&computeIndex, physicalDevice,
				VK_QUEUE_COMPUTE_BIT);
		uint32_t ret3 = getPresentQueueIndex(&presentIndex, physicalDevice,
				surface);
		/* Panic if indices are unavailable */
		if (ret1 != VK_SUCCESS || ret2 != VK_SUCCESS || ret3 != VK_SUCCESS) {
			errLog(FATAL, "unable to acquire indices\n");
			panic();
		}
	}

	/* Set extent (for now just window width and height) */
	VkExtent2D swapChainExtent = { WINDOW_WIDTH, WINDOW_HEIGHT };

	/*create device */
	VkDevice device;
	new_Device(&device, deviceInfo, physicalDevice, graphicsIndex,
			deviceExtensionCount,
			ppDeviceExtensionNames, layerCount, ppLayerNames);

	/* get swap chain details */
	struct SwapChainInfo swapChainInfo;
	new_SwapChainInfo(&swapChainInfo, physicalDevice, surface);

	/*Create swap chain */
	VkSwapchainKHR swapChain;
	new_SwapChain(&swapChain, VK_NULL_HANDLE, swapChainInfo, device, surface,
			swapChainExtent, graphicsIndex,
			presentIndex);

	uint32_t swapChainImageCount = 0;
	VkImageView* pSwapChainImages = NULL;
	VkImageView* pSwapChainImageViews = NULL;
	new_SwapChainImages(&swapChainImageCount, &pSwapChainImages, device,
			swapChain);
	new_SwapChainImageViews(&pSwapChainImageViews, device, swapChainInfo.preferredFormat.format,
			swapChainImageCount,
			pSwapChainImages);


	VkShaderModule fragShaderModule;
	{
		uint32_t* fragShaderFileContents;
		uint32_t fragShaderFileLength;
		readShaderFile("assets/shaders/shader.frag.spv", &fragShaderFileLength,
				&fragShaderFileContents);
		new_ShaderModule(&fragShaderModule, device, fragShaderFileLength,
				fragShaderFileContents);
		free(fragShaderFileContents);
	}

	VkShaderModule vertShaderModule;
	{
		uint32_t* vertShaderFileContents;
		uint32_t vertShaderFileLength;
		readShaderFile("assets/shaders/shader.vert.spv", &vertShaderFileLength,
				&vertShaderFileContents);
		new_ShaderModule(&vertShaderModule, device, vertShaderFileLength,
				vertShaderFileContents);
		free(vertShaderFileContents);
	}

	/* Create graphics pipeline */
	VkRenderPass renderPass;
	new_RenderPass(&renderPass, device, swapChainInfo.preferredFormat.format);

	VkPipelineLayout graphicsPipelineLayout;
	new_PipelineLayout(&graphicsPipelineLayout, device);

	VkPipeline graphicsPipeline;
	new_GraphicsPipeline(&graphicsPipeline, device, vertShaderModule,
			fragShaderModule, swapChainExtent, renderPass,
			graphicsPipelineLayout);
	delete_ShaderModule(&fragShaderModule, device);
	delete_ShaderModule(&vertShaderModule, device);

	VkFramebuffer* pSwapChainFramebuffers;
	new_SwapChainFramebuffers(&pSwapChainFramebuffers, device, renderPass,
			swapChainExtent, swapChainImageCount, pSwapChainImageViews);

	VkCommandPool commandPool;
	new_CommandPool(&commandPool, device, graphicsIndex);

	VkCommandBuffer *pGraphicsCommandBuffers;
	new_GraphicsCommandBuffers(&pGraphicsCommandBuffers, device, renderPass,
			graphicsPipeline, commandPool, swapChainExtent, swapChainImageCount,
			pSwapChainFramebuffers);





	/*wait till close*/
	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
	}

	/*cleanup*/
	delete_GraphicsCommandBuffers(&pGraphicsCommandBuffers);
	delete_CommandPool(&commandPool, device);
	delete_SwapChainFramebuffers(&pSwapChainFramebuffers, swapChainImageCount,
			device);
	delete_Pipeline(&graphicsPipeline, device);
	delete_PipelineLayout(&graphicsPipelineLayout, device);
	delete_RenderPass(&renderPass, device);
	delete_SwapChainImageViews(&pSwapChainImageViews, swapChainImageCount,
			device);
	delete_SwapChainImages(&pSwapChainImages);
	delete_SwapChain(&swapChain, device);
	delete_SwapChainInfo(&swapChainInfo);
	delete_Device(&device);
	delete_DeviceInfo(&deviceInfo);
	delete_DebugCallback(&callback, instance);
	delete_Instance(&instance);
	free(ppExtensionNames);
	glfwTerminate();
	return (EXIT_SUCCESS);
}
