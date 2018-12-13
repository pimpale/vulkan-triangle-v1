
/*
 * util_methods.h
 *
 *  Created on: Dec 2, 2018
 *      Author: gpi
 */

#ifndef VULKAN_UTILS_H_
#define VULKAN_UTILS_H_

#include <stdint.h>
#include <vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

struct VulkanWindow {
	VkInstance instance;
	VkPhysicalDevice physicalDevice;

	GLFWwindow* pWindow;
	VkSurfaceKHR surface;

	uint32_t graphicsIndex;
	uint32_t computeIndex;
	uint32_t presentIndex;

	VkQueue graphicsQueue;
	VkQueue computeQueue;
	VkQueue presentQueue;

	VkDevice device;

	VkSurfaceFormatKHR surfaceFormat;

	VkExtent2D swapChainExtent;
	VkSwapchainKHR swapChain;

	uint32_t swapChainImageCount;
	/* One for each frame in the swap chain*/
	VkImage* pSwapChainImages;
	VkImageView* pSwapChainImageViews;
	VkFramebuffer* pSwapChainFramebuffers;
	VkCommandBuffer* pGraphicsCommandBuffers;
	VkSemaphore* pImageAvailableSemaphores;
	VkSemaphore* pRenderFinishedSemaphores;
	VkFence* pInFlightFences;
};

uint32_t new_GLFWwindow(const GLFWwindow** ppGLFWwindow);

uint32_t new_Surface(VkSurfaceKHR *pSurface, const GLFWwindow *pWindow,
		const VkInstance instance);

#endif VULKAN_UTILS_H_
