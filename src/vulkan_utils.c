/*
 * vulkan_utils.c
 *
 *  Created on: Dec 9, 2018
 *      Author: gpi
 */

#include <stdint.h>

#include <vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include "constants.h"
#include "errors.h"
#include "vulkan_helper.h"

#include "vulkan_utils.h"


VkExtent2D getWindowExtent(const GLFWwindow* pWindow) {
	int height;
	int width;
	glfwGetWindowSize(pWindow, &width, &height);
	VkExtent2D extent = {(uint32_t)width, (uint32_t)height};
	return (extent);
}

uint32_t new_GLFWwindow(GLFWwindow** ppGLFWwindow) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	*ppGLFWwindow =
	    glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPNAME, NULL, NULL);
	if (*ppGLFWwindow == NULL) {
		return (EOPFAIL);
	}
	return (ESUCCESS);
}

uint32_t new_Surface(VkSurfaceKHR* pSurface, GLFWwindow* pWindow,
		     const VkInstance instance) {
	VkResult res =
	    glfwCreateWindowSurface(instance, pWindow, NULL, pSurface);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create surface, quitting");
		panic();
	}
	return (ESUCCESS);
}

/* find queues on graphics device */
void setupVulkanWindow_getQueues(struct VulkanWindow* pVulkanWindow) {
	uint32_t getGraphicsQueueIndexRetVal = getDeviceQueueIndex(
	    &pVulkanWindow->graphicsIndex, pVulkanWindow->physicalDevice,
	    VK_QUEUE_GRAPHICS_BIT);
	uint32_t getComputeQueueIndexRetVal = getDeviceQueueIndex(
	    &pVulkanWindow->computeIndex, pVulkanWindow->physicalDevice,
	    VK_QUEUE_COMPUTE_BIT);
	uint32_t getPresentQueueIndexRetVal = getPresentQueueIndex(
	    &pVulkanWindow->presentIndex, pVulkanWindow->physicalDevice,
	    pVulkanWindow->surface);

	if (getGraphicsQueueIndexRetVal != VK_SUCCESS ||
	    getComputeQueueIndexRetVal != VK_SUCCESS ||
	    getPresentQueueIndexRetVal != VK_SUCCESS) {
		errLog(FATAL,
		       "failed to create queues, device does not appear to "
		       "support necessary operations");
		panic();
	}
}

void setupVulkanWindow_createSwapChain(struct VulkanWindow* pVulkanWindow,
				       VkSwapchainKHR* pOldSwapChain) {
	/* Set extent (for now just window width and height) */
	pVulkanWindow->swapChainExtent =
	    getWindowExtent(pVulkanWindow->pWindow);

	/* get preferred format of screen*/
	getPreferredSurfaceFormat(&pVulkanWindow->surfaceFormat,
				  pVulkanWindow->physicalDevice,
				  pVulkanWindow->surface);

	/*Create swap chain */
	new_SwapChain(
	    &pVulkanWindow->swapChain, &pVulkanWindow->swapChainImageCount,
	    *pOldSwapChain, pVulkanWindow->surfaceFormat,
	    pVulkanWindow->physicalDevice, pVulkanWindow->device,
	    pVulkanWindow->surface, pVulkanWindow->swapChainExtent,
	    pVulkanWindow->graphicsIndex, pVulkanWindow->presentIndex);

	/* Swap chain images */
	new_SwapChainImages(&pVulkanWindow->pSwapChainImages,
			    &pVulkanWindow->swapChainImageCount,
			    pVulkanWindow->device, pVulkanWindow->swapChain);
	/* Swap chain image views */
	new_SwapChainImageViews(&pVulkanWindow->pSwapChainImageViews,
				pVulkanWindow->device,
				pVulkanWindow->surfaceFormat.format,
				pVulkanWindow->swapChainImageCount,
				pVulkanWindow->pSwapChainImages);
}

uint32_t new_VulkanWindow(struct VulkanWindow* pVulkanWindow,
			  const VkInstance instance,
			  const VkPhysicalDevice physicalDevice,
			  const uint32_t enabledLayerCount,
			  const char* const* ppEnabledLayerNames) {
	pVulkanWindow->instance = instance;
	pVulkanWindow->physicalDevice = physicalDevice;

	/* Create window and surface */
	new_GLFWwindow(&pVulkanWindow->pWindow);
	new_Surface(&pVulkanWindow->surface, pVulkanWindow->pWindow,
		    pVulkanWindow->instance);

	setupVulkanWindow_getQueues(pVulkanWindow);

	/*create device */
	uint32_t deviceExtensionCount = 1;
	const char* ppDeviceExtensionNames[] = {
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	VkResult newDeviceRetVal = new_Device(
	    &pVulkanWindow->device, pVulkanWindow->physicalDevice,
	    pVulkanWindow->graphicsIndex, deviceExtensionCount,
	    ppDeviceExtensionNames, enabledLayerCount, ppEnabledLayerNames);

	if (newDeviceRetVal != VK_SUCCESS) {
		errLog(FATAL, "failed to create logical device: %s",
		       vkstrerror(newDeviceRetVal));
		panic();
	}

	setupVulkanWindow_createSwapChain(pVulkanWindow, VK_NULL_HANDLE);

	return (VK_SUCCESS);
}

