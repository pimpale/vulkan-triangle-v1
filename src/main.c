#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "constants.h"
#include "errors.h"
#include "utils.h"
#include "vulkan_helper.h"

int main(void) {
	glfwInit();

	/* Extensions, Layers, and Device Extensions declared
	 */
	uint32_t extensionCount;
	char** ppExtensionNames;
	new_RequiredInstanceExtensions(&extensionCount, &ppExtensionNames);

	uint32_t layerCount;
	char** ppLayerNames;
	new_ValidationLayers(&layerCount, &ppLayerNames);

	uint32_t deviceExtensionCount;
	char** ppDeviceExtensionNames;
	new_RequiredDeviceExtensions(&deviceExtensionCount,
				     &ppDeviceExtensionNames);

	/* Create instance */
	VkInstance instance;
	new_Instance(&instance, extensionCount,
		     (const char* const*)ppExtensionNames, layerCount,
		     (const char* const*)ppLayerNames);
	VkDebugUtilsMessengerEXT callback;
	new_DebugCallback(&callback, instance);

	/* get physical device */
	VkPhysicalDevice physicalDevice;
	getPhysicalDevice(&physicalDevice, instance);

	/* Create window and surface */
	GLFWwindow* pWindow;
	new_GLFWwindow(&pWindow);
	VkSurfaceKHR surface;
	new_Surface(&surface, pWindow, instance);

	/* find queues on graphics device */
	uint32_t graphicsIndex;
	uint32_t computeIndex;
	uint32_t presentIndex;
	{
		uint32_t ret1 = getDeviceQueueIndex(
		    &graphicsIndex, physicalDevice, VK_QUEUE_GRAPHICS_BIT);
		uint32_t ret2 = getDeviceQueueIndex(
		    &computeIndex, physicalDevice, VK_QUEUE_COMPUTE_BIT);
		uint32_t ret3 = getPresentQueueIndex(&presentIndex,
						     physicalDevice, surface);
		/* Panic if indices are unavailable */
		if (ret1 != VK_SUCCESS || ret2 != VK_SUCCESS ||
		    ret3 != VK_SUCCESS) {
			errLog(FATAL, "unable to acquire indices\n");
			panic();
		}
	}

	/* Set extent (for now just window width and height) */
	VkExtent2D swapChainExtent;
	getWindowExtent(&swapChainExtent, pWindow);

	/*create device */
	VkDevice device;
	new_Device(&device, physicalDevice, graphicsIndex, deviceExtensionCount,
		   (const char* const*)ppDeviceExtensionNames, layerCount,
		   (const char* const*)ppLayerNames);

	VkQueue graphicsQueue;
	getQueue(&graphicsQueue, device, graphicsIndex);
	VkQueue computeQueue;
	getQueue(&computeQueue, device, computeIndex);
	VkQueue presentQueue;
	getQueue(&presentQueue, device, presentIndex);

	/* get preferred format of screen*/
	VkSurfaceFormatKHR surfaceFormat;
	getPreferredSurfaceFormat(&surfaceFormat, physicalDevice, surface);

	/*Create swap chain */
	VkSwapchainKHR swapChain;
	uint32_t swapChainImageCount = 0;
	new_SwapChain(&swapChain, &swapChainImageCount, VK_NULL_HANDLE,
		      surfaceFormat, physicalDevice, device, surface,
		      swapChainExtent, graphicsIndex, presentIndex);

	VkImage* pSwapChainImages = NULL;
	VkImageView* pSwapChainImageViews = NULL;

	new_SwapChainImages(&pSwapChainImages, &swapChainImageCount, device,
			    swapChain);
	new_SwapChainImageViews(&pSwapChainImageViews, device,
				surfaceFormat.format, swapChainImageCount,
				pSwapChainImages);

	VkShaderModule fragShaderModule;
	{
		uint32_t* fragShaderFileContents;
		uint32_t fragShaderFileLength;
		readShaderFile("assets/shaders/shader.frag.spv",
			       &fragShaderFileLength, &fragShaderFileContents);
		new_ShaderModule(&fragShaderModule, device,
				 fragShaderFileLength, fragShaderFileContents);
		free(fragShaderFileContents);
	}

	VkShaderModule vertShaderModule;
	{
		uint32_t* vertShaderFileContents;
		uint32_t vertShaderFileLength;
		readShaderFile("assets/shaders/shader.vert.spv",
			       &vertShaderFileLength, &vertShaderFileContents);
		new_ShaderModule(&vertShaderModule, device,
				 vertShaderFileLength, vertShaderFileContents);
		free(vertShaderFileContents);
	}

	/* Create graphics pipeline */
	VkRenderPass renderPass;
	new_RenderPass(&renderPass, device, surfaceFormat.format);

	VkPipelineLayout graphicsPipelineLayout;
	new_PipelineLayout(&graphicsPipelineLayout, device);

	VkPipeline graphicsPipeline;
	new_GraphicsPipeline(&graphicsPipeline, device, vertShaderModule,
			     fragShaderModule, swapChainExtent, renderPass,
			     graphicsPipelineLayout);

	VkFramebuffer* pSwapChainFramebuffers;
	new_SwapChainFramebuffers(&pSwapChainFramebuffers, device, renderPass,
				  swapChainExtent, swapChainImageCount,
				  pSwapChainImageViews);

	VkCommandPool commandPool;
	new_CommandPool(&commandPool, device, graphicsIndex);

	VkCommandBuffer* pGraphicsCommandBuffers;
	new_GraphicsCommandBuffers(&pGraphicsCommandBuffers, device, renderPass,
				   graphicsPipeline, commandPool,
				   swapChainExtent, swapChainImageCount,
				   pSwapChainFramebuffers);

	VkSemaphore* pImageAvailableSemaphores;
	VkSemaphore* pRenderFinishedSemaphores;
	VkFence* pInFlightFences;
	new_Semaphores(&pImageAvailableSemaphores, swapChainImageCount, device);
	new_Semaphores(&pRenderFinishedSemaphores, swapChainImageCount, device);
	new_Fences(&pInFlightFences, swapChainImageCount, device);

	uint32_t currentFrame = 0;
	/*wait till close*/
	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
		uint32_t result = drawFrame(
		    &currentFrame, 2, device, swapChain,
		    pGraphicsCommandBuffers, pInFlightFences,
		    pImageAvailableSemaphores, pRenderFinishedSemaphores,
		    graphicsQueue, presentQueue);

		if (result == EOUTOFDATE) {
			vkDeviceWaitIdle(device);
			delete_Fences(&pInFlightFences, swapChainImageCount,
				      device);
			delete_Semaphores(&pRenderFinishedSemaphores,
					  swapChainImageCount, device);
			delete_Semaphores(&pImageAvailableSemaphores,
					  swapChainImageCount, device);
			delete_GraphicsCommandBuffers(&pGraphicsCommandBuffers);
			delete_CommandPool(&commandPool, device);
			delete_SwapChainFramebuffers(&pSwapChainFramebuffers,
						     swapChainImageCount,
						     device);
			delete_Pipeline(&graphicsPipeline, device);
			delete_PipelineLayout(&graphicsPipelineLayout, device);
			delete_RenderPass(&renderPass, device);
			delete_SwapChainImageViews(&pSwapChainImageViews,
						   swapChainImageCount, device);
			delete_SwapChainImages(&pSwapChainImages);
			delete_SwapChain(&swapChain, device);

			/* Set swapchain to new window size */
			getWindowExtent(&swapChainExtent, pWindow);

			/*Create swap chain */
			new_SwapChain(
			    &swapChain, &swapChainImageCount, VK_NULL_HANDLE,
			    surfaceFormat, physicalDevice, device, surface,
			    swapChainExtent, graphicsIndex, presentIndex);
			new_SwapChainImages(&pSwapChainImages,
					    &swapChainImageCount, device,
					    swapChain);
			new_SwapChainImageViews(
			    &pSwapChainImageViews, device, surfaceFormat.format,
			    swapChainImageCount, pSwapChainImages);

			/* Create graphics pipeline */
			new_RenderPass(&renderPass, device,
				       surfaceFormat.format);
			new_PipelineLayout(&graphicsPipelineLayout, device);
			new_GraphicsPipeline(&graphicsPipeline, device,
					     vertShaderModule, fragShaderModule,
					     swapChainExtent, renderPass,
					     graphicsPipelineLayout);
			new_SwapChainFramebuffers(
			    &pSwapChainFramebuffers, device, renderPass,
			    swapChainExtent, swapChainImageCount,
			    pSwapChainImageViews);
			new_CommandPool(&commandPool, device, graphicsIndex);
			new_GraphicsCommandBuffers(
			    &pGraphicsCommandBuffers, device, renderPass,
			    graphicsPipeline, commandPool, swapChainExtent,
			    swapChainImageCount, pSwapChainFramebuffers);
			new_Semaphores(&pImageAvailableSemaphores,
				       swapChainImageCount, device);
			new_Semaphores(&pRenderFinishedSemaphores,
				       swapChainImageCount, device);
			new_Fences(&pInFlightFences, swapChainImageCount,
				   device);
		}
	}

	/*cleanup*/
	vkDeviceWaitIdle(device);
	delete_ShaderModule(&fragShaderModule, device);
	delete_ShaderModule(&vertShaderModule, device);
	delete_Fences(&pInFlightFences, swapChainImageCount, device);
	delete_Semaphores(&pRenderFinishedSemaphores, swapChainImageCount,
			  device);
	delete_Semaphores(&pImageAvailableSemaphores, swapChainImageCount,
			  device);
	delete_GraphicsCommandBuffers(&pGraphicsCommandBuffers);
	delete_CommandPool(&commandPool, device);
	delete_SwapChainFramebuffers(&pSwapChainFramebuffers,
				     swapChainImageCount, device);
	delete_Pipeline(&graphicsPipeline, device);
	delete_PipelineLayout(&graphicsPipelineLayout, device);
	delete_RenderPass(&renderPass, device);
	delete_SwapChainImageViews(&pSwapChainImageViews, swapChainImageCount,
				   device);
	delete_SwapChainImages(&pSwapChainImages);
	delete_SwapChain(&swapChain, device);
	delete_Device(&device);
	delete_Surface(&surface, instance);
	delete_DebugCallback(&callback, instance);
	delete_Instance(&instance);
	delete_RequiredDeviceExtensions(&deviceExtensionCount,
					&ppDeviceExtensionNames);
	delete_ValidationLayers(&layerCount, &ppLayerNames);
	delete_RequiredInstanceExtensions(&extensionCount, &ppExtensionNames);
	glfwTerminate();
	return (EXIT_SUCCESS);
}
