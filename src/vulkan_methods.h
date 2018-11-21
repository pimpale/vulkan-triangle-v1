/*
 * vulkan_methods.h
 *
 *  Created on: Aug 8, 2018
 *      Author: gpi
 */

#ifndef VULKAN_METHODS_H_
#define VULKAN_METHODS_H_

#include <stdbool.h>
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

struct InstanceInfo {
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


/**
 * Queue family indices
 */
struct DeviceIndices {
	uint32_t computeIndex;
	uint32_t graphicsIndex;
	uint32_t presentIndex;
	bool hasCompute;
	bool hasGraphics;
	bool hasPresent;
};

uint32_t new_InstanceInfo(struct InstanceInfo* pInstanceInfo);
uint32_t delete_InstanceInfo(struct InstanceInfo* pInstanceInfo);

uint32_t new_DeviceInfo(struct DeviceInfo* pDeviceInfo,
		VkPhysicalDevice physicalDevice);
uint32_t delete_DeviceInfo(struct DeviceInfo* pDeviceInfo);

uint32_t new_SwapChainInfo(struct SwapChainInfo *pSwapChainInfo,
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface);
void delete_SwapChainInfo(struct SwapChainInfo* pSwapChainInfo);

uint32_t new_Instance(VkInstance* pInstance, struct InstanceInfo instanceInfo,
		uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames);

void delete_Instance(VkInstance *pInstance);

uint32_t new_DebugCallback(VkDebugUtilsMessengerEXT* pCallback,
		VkInstance instance);

void delete_DebugCallback(VkDebugUtilsMessengerEXT *pCallback,
		VkInstance instance);

uint32_t getPhysicalDevice(VkPhysicalDevice* pDevice, VkInstance instance);

uint32_t new_Device(VkDevice* pDevice, struct DeviceInfo deviceInfo,
		VkPhysicalDevice physicalDevice,
		uint32_t deviceQueueIndex,
		uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames);

void delete_Device(VkDevice *pDevice);

struct DeviceIndices new_DeviceIndices(VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface);

void delete_DeviceIndices(struct DeviceIndices deviceIndices);

VkQueue getQueue(VkDevice device, uint32_t deviceQueueIndex);

VkSwapchainKHR new_SwapChain(VkSwapchainKHR oldSwapChain,
		struct SwapChainInfo swapChainInfo,
		VkDevice device,
		VkSurfaceKHR surface, VkExtent2D extent,
		struct DeviceIndices deviceIndices);

void delete_SwapChain(VkDevice device, VkSwapchainKHR swapChain);

void new_SwapChainImages(VkDevice device, VkSwapchainKHR swapChain,
		uint32_t *pImageCount, VkImage **ppSwapChainImages);

void delete_SwapChainImages(VkImage* pImages);

void new_SwapChainImageViews(VkDevice device, VkFormat format,
		uint32_t imageCount, VkImage* pSwapChainImages,
		VkImageView** ppImageViews);

void delete_SwapChainImageViews(VkDevice device, uint32_t imageCount,
		VkImageView* pImageViews);

VkShaderModule new_ShaderModule(VkDevice device, uint32_t codeSize,
		uint32_t* pCode);

void delete_ShaderModule(VkDevice device, VkShaderModule shaderModule);

VkRenderPass new_RenderPass(VkDevice device, VkFormat swapChainImageFormat);

void delete_RenderPass(VkDevice device, VkRenderPass renderPass);

void new_GraphicsPipeline(VkDevice device,
		VkShaderModule vertShaderModule, VkShaderModule fragShaderModule,
		VkExtent2D extent,
		VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
		VkPipeline* graphicsPipeline);


void delete_GraphicsPipeline(VkDevice device, VkPipeline pipeline);


#endif /* VULKAN_METHODS_H_ */
