
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>

#include "constants.h"
#include "errors.h"
#include "utils.h"

#include "vulkan_helper.h"

char* vkstrerror(VkResult err)
{
	switch (err) {
	case VK_SUCCESS:
		return ("VK_SUCCESS");
	case VK_NOT_READY:
		return ("VK_NOT_READY");
	case VK_TIMEOUT:
		return ("VK_TIMEOUT ");
	case VK_EVENT_SET:
		return ("VK_EVENT_SET ");
	case VK_EVENT_RESET:
		return ("VK_EVENT_RESET");
	case VK_INCOMPLETE:
		return ("VK_INCOMPLETE");
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return ("VK_ERROR_OUT_OF_HOST_MEMORY");
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return ("VK_ERROR_OUT_OF_DEVICE_MEMORY");
	case VK_ERROR_INITIALIZATION_FAILED:
		return ("VK_ERROR_INITIALIZATION_FAILED");
	case VK_ERROR_DEVICE_LOST:
		return ("VK_ERROR_DEVICE_LOST");
	case VK_ERROR_MEMORY_MAP_FAILED:
		return ("VK_ERROR_MEMORY_MAP_FAILED");
	case VK_ERROR_LAYER_NOT_PRESENT:
		return ("VK_ERROR_LAYER_NOT_PRESENT");
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return ("VK_ERROR_EXTENSION_NOT_PRESENT");
	case VK_ERROR_FEATURE_NOT_PRESENT:
		return ("VK_ERROR_FEATURE_NOT_PRESENT");
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return ("VK_ERROR_INCOMPATIBLE_DRIVER");
	case VK_ERROR_TOO_MANY_OBJECTS:
		return ("VK_ERROR_TOO_MANY_OBJECTS");
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return ("VK_ERROR_FORMAT_NOT_SUPPORTED");
	case VK_ERROR_FRAGMENTED_POOL:
		return ("VK_ERROR_FRAGMENTED_POOL");
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		return ("VK_ERROR_OUT_OF_POOL_MEMORY");
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		return ("VK_ERROR_INVALID_EXTERNAL_HANDLE");
	case VK_ERROR_SURFACE_LOST_KHR:
		return ("VK_ERROR_SURFACE_LOST_KHR");
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return ("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
	case VK_SUBOPTIMAL_KHR:
		return ("VK_SUBOPTIMAL_KHR");
	case VK_ERROR_OUT_OF_DATE_KHR:
		return ("VK_ERROR_OUT_OF_DATE_KHR");
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return ("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
	case VK_ERROR_VALIDATION_FAILED_EXT:
		return ("VK_ERROR_VALIDATION_FAILED_EXT");
	case VK_ERROR_INVALID_SHADER_NV:
		return ("VK_ERROR_INVALID_SHADER_NV");
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
		return ("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT");
	case VK_ERROR_FRAGMENTATION_EXT:
		return ("VK_ERROR_FRAGMENTATION_EXT");
	case VK_ERROR_NOT_PERMITTED_EXT:
		return ("VK_ERROR_NOT_PERMITTED_EXT");
	default:
		return ("UNKNOWN_ERROR");
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData) {

	/* set severity */
	uint32_t errcode;
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		errcode = INFO;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		errcode = INFO;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		errcode = WARN;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		errcode = ERROR;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		errcode = UNKNOWN;
		break;
	default:
		errcode = UNKNOWN;
		break;
	}
	/* log error */
	errLog(errcode, "Vulkan validation layer: %s\n", pCallbackData->pMessage);
	return (VK_FALSE);
}
uint32_t new_Instance(VkInstance* pInstance,
		const uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		const uint32_t enabledLayerCount,
		const char * const *ppEnabledLayerNames) {
	/* Create app info */
	VkApplicationInfo appInfo = {0};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = APPNAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "None";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	/* Create info */
	VkInstanceCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = enabledExtensionCount;
	createInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
	createInfo.enabledLayerCount = enabledLayerCount;
	createInfo.ppEnabledLayerNames = ppEnabledLayerNames;

	VkResult result = vkCreateInstance(&createInfo, NULL, pInstance);
	if (result != VK_SUCCESS) {
		errLog(FATAL, "Failed to create instance, error code: %d",
				(uint32_t) result);
		panic();
	}
	return (VK_SUCCESS);
}

void delete_Instance(VkInstance *pInstance) {
	vkDestroyInstance(*pInstance, NULL);
}

/**
 * Requires the debug utils extension
 */

uint32_t new_DebugCallback(VkDebugUtilsMessengerEXT* pCallback,
		const VkInstance instance) {
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	/* Returns a function pointer */
	PFN_vkCreateDebugUtilsMessengerEXT func =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					instance, "vkCreateDebugUtilsMessengerEXT");
	if (!func) {
		errLog(FATAL, "Failed to find extension function\n");
		panic();
	}
	VkResult result = func(instance, &createInfo, NULL, pCallback);
	if (result != VK_SUCCESS) {
		errLog(FATAL, "Failed to create debug callback, error code: %d",
				(uint32_t) result);
		panic();
	}
	return (VK_SUCCESS);
}

/**
 * Requires the debug utils extension
 */
void delete_DebugCallback(VkDebugUtilsMessengerEXT *pCallback,
		const VkInstance instance) {
	PFN_vkDestroyDebugUtilsMessengerEXT func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != NULL) {
		func(instance, *pCallback, NULL);
	}
}

uint32_t getPhysicalDevice(VkPhysicalDevice* pDevice, const VkInstance instance) {
	uint32_t deviceCount = 0;
	VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	if (res != VK_SUCCESS || deviceCount == 0) {
		errLog(FATAL, "no Vulkan capable device found");
		panic();
	}
	VkPhysicalDevice *arr = malloc(deviceCount * sizeof(VkPhysicalDevice));
	if (!arr) {
		errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
		panic();
	}
	vkEnumeratePhysicalDevices(instance, &deviceCount, arr);

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
	for (uint32_t i = 0; i < deviceCount; i++) {
		vkGetPhysicalDeviceProperties(arr[i], &deviceProperties);
		uint32_t deviceQueueIndex;
		uint32_t ret = getDeviceQueueIndex(&deviceQueueIndex, arr[i],
				VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		if (ret == VK_SUCCESS) {
			selectedDevice = arr[i];
			break;
		}
	}

	if (selectedDevice == VK_NULL_HANDLE) {
		errLog(ERROR, "no suitable Vulkan device found\n");
		panic();
	}
	free(arr);
	*pDevice = selectedDevice;
	return (VK_SUCCESS);
}

void delete_Device(VkDevice *pDevice) {
	vkDestroyDevice(*pDevice, NULL);
}

uint32_t getDeviceQueueIndex(uint32_t *deviceQueueIndex,
		VkPhysicalDevice device, VkQueueFlags bit) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	VkQueueFamilyProperties *arr =
			malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	if (!arr) {
		errLog(FATAL, "Failed to get device queue index: %s", strerror(errno));
		panic();
	}
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, arr);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (arr[i].queueCount > 0 && (arr[0].queueFlags & bit)) {
			free(arr);
			*deviceQueueIndex = i;
			return (VK_SUCCESS);
		}
	}
	free(arr);
	return (ENODEV);
}

uint32_t getPresentQueueIndex(uint32_t* pPresentQueueIndex,
		const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
	VkQueueFamilyProperties *arr =
			malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	if (!arr) {
		errLog(FATAL, "Failed to get present queue index: %s\n",
				strerror(errno));
		panic();
	}
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, arr);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		VkBool32 surfaceSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupport);
		if (surfaceSupport) {
			*pPresentQueueIndex = i;
			free(arr);
			return (VK_SUCCESS);
		}
	}
	free(arr);
	return (ENODEV);
}

uint32_t new_Device(VkDevice* pDevice,
		const VkPhysicalDevice physicalDevice, const uint32_t deviceQueueIndex,
		const uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		const uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames) {

	VkPhysicalDeviceFeatures deviceFeatures = {0};
	VkDeviceQueueCreateInfo queueCreateInfo = {0};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = deviceQueueIndex;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = enabledExtensionCount;
	createInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
	createInfo.enabledLayerCount = enabledLayerCount;
	createInfo.ppEnabledLayerNames = ppEnabledLayerNames;

	VkResult res = vkCreateDevice(physicalDevice, &createInfo, NULL, pDevice);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "Failed to create device, error code: %d",
				(uint32_t) res);
		panic();
	}
	return (VK_SUCCESS);
}

uint32_t getQueue(VkQueue* pQueue, const VkDevice device,
		const uint32_t deviceQueueIndex) {
	vkGetDeviceQueue(device, deviceQueueIndex, 0, pQueue);
	return (VK_SUCCESS);
}

uint32_t new_SwapChain(VkSwapchainKHR* pSwapChain,
		uint32_t *pSwapChainImageCount,
		const VkSwapchainKHR oldSwapChain,
		const VkSurfaceFormatKHR surfaceFormat,
		const VkPhysicalDevice physicalDevice, const VkDevice device,
		const VkSurfaceKHR surface, const VkExtent2D extent,
		const uint32_t graphicsIndex, const uint32_t presentIndex) {

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
			&capabilities);

	*pSwapChainImageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0
			&& *pSwapChainImageCount > capabilities.maxImageCount) {
		*pSwapChainImageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = *pSwapChainImageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { graphicsIndex, presentIndex };
	if (graphicsIndex != presentIndex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; /* Optional */
		createInfo.pQueueFamilyIndices = NULL; /* Optional */
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	/* guaranteed to be available */
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = oldSwapChain;
	VkResult res = vkCreateSwapchainKHR(device, &createInfo, NULL, pSwapChain);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "Failed to create swap chain, error code: %d",
				(uint32_t) res);
		panic();
	}
	return (VK_SUCCESS);
}

void delete_SwapChain(VkSwapchainKHR *pSwapChain, const VkDevice device) {
	vkDestroySwapchainKHR(device, *pSwapChain, NULL);
}


uint32_t getPreferredSurfaceFormat(VkSurfaceFormatKHR* pSurfaceFormat,
		const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
	uint32_t formatCount;
	VkSurfaceFormatKHR *pSurfaceFormats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
			NULL);
	if (formatCount != 0) {
		pSurfaceFormats = malloc(formatCount * sizeof(VkSurfaceFormatKHR));
		if (!pSurfaceFormats) {
			errLog(FATAL, "could not get preferred format: %s",
					strerror(errno));
			panic();
		}
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
				&formatCount, pSurfaceFormats);
	} else {
		pSurfaceFormats = NULL;
	}

	VkSurfaceFormatKHR preferredFormat;
	if (formatCount == 1 && pSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		/* If it has no preference, use our own */
		preferredFormat = pSurfaceFormats[0];
	} else if (formatCount != 0) {
		/* we default to the first one in the list */
		preferredFormat = pSurfaceFormats[0];
		/* However,  we check to make sure that what we want is in there */
		for (uint32_t i = 0; i < formatCount; i++) {
			VkSurfaceFormatKHR availableFormat = pSurfaceFormats[i];
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
					&& availableFormat.colorSpace
							== VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				preferredFormat = availableFormat;
			}
		}
	} else {
		errLog(ERROR, "no formats available\n");
		panic();
	}

	free(pSurfaceFormats);

	*pSurfaceFormat = preferredFormat;
	return (VK_SUCCESS);
}


uint32_t new_SwapChainImages(
		VkImage **ppSwapChainImages,
		uint32_t *pImageCount,
		const VkDevice device, const VkSwapchainKHR swapChain) {
	vkGetSwapchainImagesKHR(device, swapChain, pImageCount, NULL);
	*ppSwapChainImages = malloc((*pImageCount) * sizeof(VkImage));
	if (!*ppSwapChainImages) {
		errLog(FATAL, "failed to get swap chain images: %s\n", strerror(errno));
		panic();
	}
	VkResult res = vkGetSwapchainImagesKHR(device, swapChain, pImageCount,
			*ppSwapChainImages);

	return (VK_SUCCESS);
}

void delete_SwapChainImages(VkImage **ppImages) {
	free(*ppImages);
	*ppImages = NULL;
}

uint32_t new_ImageView(VkImageView *pImageView, const VkDevice device,
		const VkImage image, const VkFormat format) {
	VkImageViewCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	VkResult ret = vkCreateImageView(device, &createInfo, NULL,
			pImageView);
	if (ret != VK_SUCCESS) {
		errLog(FATAL, "could not create image view, error code: %d",
				(uint32_t) ret);
		panic();
	}
	return (VK_SUCCESS);
}

void delete_ImageView(VkImageView *pImageView, VkDevice device) {
	vkDestroyImageView(device, *pImageView, NULL);
}

uint32_t new_SwapChainImageViews(VkImageView** ppImageViews,
		const VkDevice device, const VkFormat format, const uint32_t imageCount,
		const VkImage* pSwapChainImages) {
	VkImageView* pImageViews = malloc(imageCount * sizeof(VkImageView));
	if (!pImageViews) {
		errLog(FATAL, "could not create swap chain image views: %s",
				strerror(errno));
		panic();
	}

	for (uint32_t i = 0; i < imageCount; i++) {
		uint32_t ret = new_ImageView(&(pImageViews[i]), device,
				pSwapChainImages[i], format);
		if (ret != VK_SUCCESS) {
			errLog(FATAL, "could not create image view, error code: %d",
					(uint32_t) ret);
			panic();
		}
	}

	*ppImageViews = pImageViews;
	return (VK_SUCCESS);
}

void delete_SwapChainImageViews(VkImageView** ppImageViews, uint32_t imageCount,
		const VkDevice device) {
	for (uint32_t i = 0; i < imageCount; i++) {
		delete_ImageView(&((*ppImageViews)[i]), device);
	}
	free(*ppImageViews);
	*ppImageViews = NULL;
}


uint32_t new_ShaderModule(VkShaderModule *pShaderModule, const VkDevice device,
		const uint32_t codeSize, const uint32_t* pCode) {
	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeSize;
	createInfo.pCode = pCode;
	VkResult res = vkCreateShaderModule(device, &createInfo, NULL,
			pShaderModule);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create shader module");
	}
	return (VK_SUCCESS);
}

void delete_ShaderModule(VkShaderModule* pShaderModule, const VkDevice device) {
	vkDestroyShaderModule(device, *pShaderModule, NULL);
}

uint32_t new_RenderPass(VkRenderPass* pRenderPass, const VkDevice device,
		const VkFormat swapChainImageFormat) {
	VkAttachmentDescription colorAttachment = { 0 };
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = { 0 };
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = { 0 };
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = { 0 };
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
			| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult res = vkCreateRenderPass(device, &renderPassInfo, NULL,
			pRenderPass);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "Could not create render pass, error: %d\n",
				(uint32_t) res);
		panic();
	}
	return (VK_SUCCESS);
}

void delete_RenderPass(VkRenderPass *pRenderPass, const VkDevice device) {
	vkDestroyRenderPass(device, *pRenderPass, NULL);
}

uint32_t new_PipelineLayout(VkPipelineLayout *pPipelineLayout,
		const VkDevice device) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	VkResult res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL,
			pPipelineLayout);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create pipeline layout with error: %d\n",
				(uint32_t) res);
		panic();
	}
	return (VK_SUCCESS);
}

void delete_PipelineLayout(VkPipelineLayout *pPipelineLayout,
		const VkDevice device) {
	vkDestroyPipelineLayout(device, *pPipelineLayout, NULL);
}


uint32_t new_GraphicsPipeline(VkPipeline* pGraphicsPipeline,
		const VkDevice device, const VkShaderModule vertShaderModule,
		const VkShaderModule fragShaderModule, const VkExtent2D extent,
		const VkRenderPass renderPass, const VkPipelineLayout pipelineLayout) {

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = { 0 };
	vertShaderStageInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = { 0 };
	fragShaderStageInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,
			fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = { 0 };
	vertexInputInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
	inputAssembly.sType =
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = { 0 };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) extent.width;
	viewport.height = (float) extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = { 0 };
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportState = { 0 };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
	rasterizer.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
	multisampling.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
			| VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
	colorBlending.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
	NULL, pGraphicsPipeline) != VK_SUCCESS) {
		errLog(FATAL, "failed to create graphics pipeline!\n");
		panic();
	}
	return (VK_SUCCESS);
}

void delete_Pipeline(VkPipeline *pPipeline, const VkDevice device) {
	vkDestroyPipeline(device, *pPipeline, NULL);
}

uint32_t new_Framebuffer(VkFramebuffer *pFramebuffer, const VkDevice device,
		const VkRenderPass renderPass, const VkImageView imageView,
		const VkExtent2D swapChainExtent) {
	VkFramebufferCreateInfo framebufferInfo = { 0 };
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = (VkImageView[] ) { imageView };
	framebufferInfo.width = swapChainExtent.width;
	framebufferInfo.height = swapChainExtent.height;
	framebufferInfo.layers = 1;
	VkResult res = vkCreateFramebuffer(device, &framebufferInfo, NULL,
			pFramebuffer);
	return ((uint32_t) res);
}

void delete_Framebuffer(VkFramebuffer *pFramebuffer, VkDevice device) {
	vkDestroyFramebuffer(device, *pFramebuffer, NULL);
}


uint32_t new_SwapChainFramebuffers(VkFramebuffer** ppFramebuffers,
		const VkDevice device, const VkRenderPass renderPass,
		const VkExtent2D swapChainExtent, const uint32_t imageCount,
		const VkImageView* pSwapChainImageViews) {
	VkFramebuffer* tmp = malloc(imageCount * sizeof(VkFramebuffer));
	if (!tmp) {
		errLog(FATAL, "could not create framebuffers: %s",
				strerror(errno));
		panic();
	}

	for (uint32_t i = 0; i < imageCount; i++) {
		uint32_t res = new_Framebuffer(&(tmp[i]), device, renderPass,
				pSwapChainImageViews[i], swapChainExtent);
		if (res != VK_SUCCESS) {
			errLog(ERROR, "could not create framebuffer, error code: %d", res);
			return (res);
		}
	}

	*ppFramebuffers = tmp;
	return (VK_SUCCESS);
}

void delete_SwapChainFramebuffers(VkFramebuffer** ppFramebuffers,
		const uint32_t imageCount, const VkDevice device) {
	for (uint32_t i = 0; i < imageCount; i++) {
		delete_Framebuffer(&((*ppFramebuffers)[i]), device);
	}
	free(*ppFramebuffers);
	*ppFramebuffers = NULL;
}

uint32_t new_CommandPool(VkCommandPool *pCommandPool, const VkDevice device,
		const uint32_t queueFamilyIndex) {
	VkCommandPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = 0;
	VkResult ret = vkCreateCommandPool(device, &poolInfo, NULL, pCommandPool);
	return (ret);
}

void delete_CommandPool(VkCommandPool *pCommandPool, const VkDevice device) {
	vkDestroyCommandPool(device, *pCommandPool, NULL);
}

uint32_t new_GraphicsCommandBuffers(VkCommandBuffer **ppCommandBuffers,
		const VkDevice device, const VkRenderPass renderPass,
		const VkPipeline graphicsPipeline, const VkCommandPool commandPool,
		const VkExtent2D swapChainExtent,
		const uint32_t swapChainFramebufferCount,
		const VkFramebuffer* pSwapChainFramebuffers) {
	VkCommandBufferAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = swapChainFramebufferCount;

	VkCommandBuffer* pCommandBuffers = malloc(
			swapChainFramebufferCount * sizeof(VkCommandBuffer));
	if (!pCommandBuffers) {
		errLog(FATAL, "Failed to create graphics command buffers: %s\n",
				strerror(errno));
		panic();
	}

	VkResult ret = vkAllocateCommandBuffers(device, &allocInfo,
			pCommandBuffers);
	if (ret != VK_SUCCESS) {
		errLog(FATAL,
				"Failed to create graphics command buffers, error code: %d\n",
				(uint32_t) ret);
		panic();
	}

	for (size_t i = 0; i < swapChainFramebufferCount; i++) {
		VkCommandBufferBeginInfo beginInfo = { 0 };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(pCommandBuffers[i], &beginInfo)
				!= VK_SUCCESS) {
			errLog(FATAL, "Failed to record into graphics command buffer\n");
			panic();
		}

		VkRenderPassBeginInfo renderPassInfo = { 0 };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = pSwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = (VkOffset2D ) { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(pCommandBuffers[i], &renderPassInfo,
				VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
				graphicsPipeline);

		vkCmdDraw(pCommandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(pCommandBuffers[i]);

		VkResult ret = vkEndCommandBuffer(pCommandBuffers[i]);
		if (ret != VK_SUCCESS)
		{
			errLog(FATAL, "Failed to record command buffer, error code: %d",
					(uint32_t) ret);
			panic();
		}
	}

	*ppCommandBuffers = pCommandBuffers;
	return (VK_SUCCESS);
}

void delete_GraphicsCommandBuffers(VkCommandBuffer **ppCommandBuffers) {
	free(*ppCommandBuffers);
	*ppCommandBuffers = NULL;
}

uint32_t create_Semaphore(VkSemaphore* semaphore, const VkDevice device) {
	VkSemaphoreCreateInfo semaphoreInfo = { 0 };
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	return (vkCreateSemaphore(device, &semaphoreInfo, NULL, semaphore));
}

void delete_Semaphore(VkSemaphore* semaphore, const VkDevice device) {
	vkDestroySemaphore(device, *semaphore, NULL);
}

uint32_t drawFrame(const VkDevice device, const VkSwapchainKHR swapChain,
		const VkCommandBuffer *pCommandBuffers,
		const VkSemaphore imageAvailableSemaphore,
		const VkSemaphore renderFinishedSemaphore, const VkQueue graphicsQueue,
		const VkQueue presentQueue) {
	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
			imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &pCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult res = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "failed to draw frame: failed to submit queue: %s",
				vkstrerror(res));
		panic();
	}

	VkSubpassDependency dependency = { 0 };
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
			| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkPresentInfoKHR presentInfo = { 0 };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	/*optional*/
	presentInfo.pResults = NULL;

	vkQueuePresentKHR(presentQueue, &presentInfo);

	return (VK_SUCCESS);
}

