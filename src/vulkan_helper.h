/*
 * vulkan_methods.h
 *
 *  Created on: Aug 8, 2018
 *      Author: gpi
 */

#ifndef VULKAN_HELPER_H_
#define VULKAN_HELPER_H_

#include <stdint.h>
#include <vulkan.h>

uint32_t new_Instance(VkInstance* pInstance,
		const uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		const uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames);

void delete_Instance(VkInstance *pInstance);

uint32_t new_DebugCallback(VkDebugUtilsMessengerEXT* pCallback,
		const VkInstance instance);

void delete_DebugCallback(VkDebugUtilsMessengerEXT *pCallback,
		const VkInstance instance);

uint32_t getPhysicalDevice(VkPhysicalDevice* pDevice, const VkInstance instance);

uint32_t new_Device(VkDevice* pDevice,
		const VkPhysicalDevice physicalDevice, const uint32_t deviceQueueIndex,
		const uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		const uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames);

void delete_Device(VkDevice *pDevice);

uint32_t getDeviceQueueIndex(uint32_t *deviceQueueIndex,
		VkPhysicalDevice device, VkQueueFlags bit);

uint32_t getPresentQueueIndex(uint32_t* pPresentQueueIndex,
		const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface);

uint32_t getQueue(VkQueue* pQueue, const VkDevice device,
		const uint32_t deviceQueueIndex);

uint32_t getPreferredSurfaceFormat(VkSurfaceFormatKHR* pSurfaceFormat,
		const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface);

uint32_t new_SwapChain(VkSwapchainKHR* pSwapChain,
		uint32_t *pSwapChainImageCount,
		const VkSwapchainKHR oldSwapChain,
		const VkSurfaceFormatKHR surfaceFormat,
		const VkPhysicalDevice physicalDevice, const VkDevice device,
		const VkSurfaceKHR surface, const VkExtent2D extent,
		const uint32_t graphicsIndex, const uint32_t presentIndex);


void delete_SwapChain(VkSwapchainKHR* pSwapChain, const VkDevice device);

uint32_t new_SwapChainImages(VkImage **ppSwapChainImages, uint32_t *pImageCount,
		const VkDevice device, const VkSwapchainKHR swapChain);

void delete_SwapChainImages(VkImage** ppImages);

uint32_t new_SwapChainImageViews(VkImageView** ppImageViews,
		const VkDevice device, const VkFormat format, const uint32_t imageCount,
		const VkImage* pSwapChainImages);

void delete_SwapChainImageViews(VkImageView** ppImageViews, uint32_t imageCount,
		const VkDevice device);

uint32_t new_ShaderModule(VkShaderModule *pShaderModule, const VkDevice device,
		const uint32_t codeSize, const uint32_t* pCode);

void delete_ShaderModule(VkShaderModule* pShaderModule, const VkDevice device);

uint32_t new_RenderPass(VkRenderPass* pRenderPass, const VkDevice device,
		const VkFormat swapChainImageFormat);

void delete_RenderPass(VkRenderPass *pRenderPass, const VkDevice device);

uint32_t new_PipelineLayout(VkPipelineLayout *pPipelineLayout,
		const VkDevice device);

void delete_PipelineLayout(VkPipelineLayout *pPipelineLayout,
		const VkDevice device);

uint32_t new_GraphicsPipeline(VkPipeline* pGraphicsPipeline,
		const VkDevice device, const VkShaderModule vertShaderModule,
		const VkShaderModule fragShaderModule, const VkExtent2D extent,
		const VkRenderPass renderPass, const VkPipelineLayout pipelineLayout);

void delete_Pipeline(VkPipeline *pPipeline, const VkDevice device);

uint32_t new_SwapChainFramebuffers(VkFramebuffer** ppFramebuffers,
		const VkDevice device, const VkRenderPass renderPass,
		const VkExtent2D swapChainExtent, const uint32_t imageCount,
		const VkImageView* pSwapChainImageViews);


void delete_SwapChainFramebuffers(VkFramebuffer** ppFramebuffers,
		const uint32_t imageCount, const VkDevice device);

uint32_t new_CommandPool(VkCommandPool *pCommandPool, const VkDevice device,
		const uint32_t queueFamilyIndex);

void delete_CommandPool(VkCommandPool *pCommandPool, const VkDevice device);

uint32_t new_GraphicsCommandBuffers(VkCommandBuffer **ppCommandBuffers,
		const VkDevice device, const VkRenderPass renderPass,
		const VkPipeline graphicsPipeline, const VkCommandPool commandPool,
		const VkExtent2D swapChainExtent,
		const uint32_t swapChainFramebufferCount,
		const VkFramebuffer* pSwapChainFramebuffers);

void delete_GraphicsCommandBuffers(VkCommandBuffer **ppCommandBuffers);

uint32_t new_Semaphore(VkSemaphore* pSemaphore, const VkDevice device);

void delete_Semaphore(VkSemaphore* pSemaphore, const VkDevice device);

uint32_t new_Semaphores(VkSemaphore** ppSemaphores,
		const uint32_t semaphoreCount, const VkDevice device);

void delete_Semaphores(VkSemaphore** ppSemaphores,
		const uint32_t semaphoreCount, const VkDevice device);

uint32_t new_Fences(VkFence **ppFences, const uint32_t fenceCount,
		const VkDevice device);

void delete_Fences(VkFence **ppFences, const uint32_t fenceCount,
		const VkDevice device);


uint32_t drawFrame(uint32_t* pCurrentFrame, const uint32_t maxFramesInFlight,
		const VkDevice device,
		const VkSwapchainKHR swapChain,
		const VkCommandBuffer *pCommandBuffers,
		const VkFence *pInFlightFences,
		const VkSemaphore *pImageAvailableSemaphores,
		const VkSemaphore *pRenderFinishedSemaphores,
		const VkQueue graphicsQueue,
		const VkQueue presentQueue);

#endif /* VULKAN_HELPER_H_ */
