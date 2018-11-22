/*
 * vulkan_methods.h
 *
 *  Created on: Aug 8, 2018
 *      Author: gpi
 */

#ifndef VULKAN_METHODS_H_
#define VULKAN_METHODS_H_

#include <stdint.h>
#include <vulkan.h>

#define NULL_INDEX -1


/**
 * This struct contains data for a physical
 * device so that it doesn't have to continuously be fetched
 */
struct DeviceInfo {
	/* all arrays will be malloc'ed and freed */
	/* at new_DeviceInfo and delete_DeviceInfo, respectively */
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	uint32_t extensionCount;
	char** ppExtensionNames;
	uint32_t layerCount;
	char** ppLayerNames;
};

struct SwapChainInfo {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	uint32_t formatCount;
	VkSurfaceFormatKHR* pFormats;
	uint32_t presentModeCount;
	VkPresentModeKHR* pPresentModes;
	VkSurfaceFormatKHR preferredFormat;
};

uint32_t new_DeviceInfo(struct DeviceInfo* pDeviceInfo,
		VkPhysicalDevice physicalDevice);
void delete_DeviceInfo(struct DeviceInfo* pDeviceInfo);

uint32_t new_SwapChainInfo(struct SwapChainInfo *pSwapChainInfo,
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface);
void delete_SwapChainInfo(struct SwapChainInfo* pSwapChainInfo);

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

uint32_t new_Device(VkDevice* pDevice, const struct DeviceInfo deviceInfo,
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

uint32_t new_SwapChain(VkSwapchainKHR* pSwapChain,
		const VkSwapchainKHR oldSwapChain,
		const struct SwapChainInfo swapChainInfo, const VkDevice device,
		const VkSurfaceKHR surface, const VkExtent2D extent,
		const uint32_t graphicsIndex, const uint32_t presentIndex);

void delete_SwapChain(VkSwapchainKHR* pSwapChain, const VkDevice device);

uint32_t new_SwapChainImages(uint32_t *pImageCount, VkImage **ppSwapChainImages,
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


#endif /* VULKAN_METHODS_H_ */
