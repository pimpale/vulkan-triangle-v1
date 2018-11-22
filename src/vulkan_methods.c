#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>

#include "util_methods.h"
#include "constants.h"
#include "error_methods.h"
#include "vulkan_methods.h"

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
	/* check layers and extensions */
	{
		uint32_t layerCount;
		uint32_t extensionCount;
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);
		vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

		/* alloc arrays */
		VkLayerProperties* pLayerProperties = malloc(
				layerCount * sizeof(VkLayerProperties));
		VkExtensionProperties* pExtensionProperties = malloc(
				extensionCount * sizeof(VkExtensionProperties));

		/* enumerate */
		vkEnumerateInstanceLayerProperties(&layerCount, pLayerProperties);
		vkEnumerateInstanceExtensionProperties(NULL, &extensionCount,
				pExtensionProperties);

		for (uint32_t x = 0; x < enabledExtensionCount; x++) {
			int found = 0;
			for (uint32_t y = 0; y < extensionCount; y++) {
				if (strncmp(ppEnabledExtensionNames[x],
						pExtensionProperties[y].extensionName,
						VK_MAX_EXTENSION_NAME_SIZE)) {
					found = 1;
					break;
				}
			}
			if (!found) {
				errLog(FATAL, "requested extension \"%s\" could not be found\n",
						ppEnabledExtensionNames[x]);
				panic();
			}
		}

		for (uint32_t x = 0; x < enabledLayerCount; x++) {
			int found = 0;
			for (uint32_t y = 0; y < layerCount; y++) {
				if (strncmp(ppEnabledLayerNames[x],
						pLayerProperties[y].layerName,
						VK_MAX_EXTENSION_NAME_SIZE)) {
					found = 1;
					break;
				}
			}
			if (!found) {
				errLog(FATAL, "requested layer \"%s\" could not be found\n",
						ppEnabledLayerNames[x]);
				panic();
			}
		}
	}
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


uint32_t new_DeviceInfo(struct DeviceInfo* pDeviceInfo,
		const VkPhysicalDevice physicalDevice) {
	/* Instantiate DeviceInfo */
	struct DeviceInfo info;
	/* Set device properties and features */
	vkGetPhysicalDeviceProperties(physicalDevice, &info.deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &info.deviceFeatures);

	/* set extension and layer counts */
	vkEnumerateDeviceLayerProperties(physicalDevice, &info.layerCount,
			NULL);
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL,
			&info.extensionCount, NULL);

	/* alloc count dependent info */
	info.ppLayerNames = malloc(info.layerCount * sizeof(char*));
	info.ppExtensionNames = malloc(info.extensionCount * sizeof(char*));
	VkLayerProperties* pLayerProperties = malloc(
			info.layerCount * sizeof(VkLayerProperties));
	VkExtensionProperties* pExtensionProperties = malloc(
			info.extensionCount * sizeof(VkExtensionProperties));

	if (!info.ppExtensionNames || !info.ppLayerNames || !pLayerProperties
			|| !pExtensionProperties) {
		errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
		panic();
	}

	/* grab values */
	vkEnumerateDeviceLayerProperties(physicalDevice, &info.layerCount,
			pLayerProperties);
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL,
			&info.extensionCount,
			pExtensionProperties);

	/* copy names to info, mallocing as we go. */
	for (uint32_t i = 0; i < info.layerCount; i++) {
		info.ppLayerNames[i] = malloc(
		VK_MAX_EXTENSION_NAME_SIZE * sizeof(char));
		if (!info.ppLayerNames[i]) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			panic();
		}
		strncpy(info.ppLayerNames[i], pLayerProperties[i].layerName,
		VK_MAX_EXTENSION_NAME_SIZE);
	}
	for (uint32_t i = 0; i < info.extensionCount; i++) {
		info.ppExtensionNames[i] = malloc(
		VK_MAX_EXTENSION_NAME_SIZE * sizeof(char));
		if (!info.ppExtensionNames[i]) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			panic();
		}
		strncpy(info.ppExtensionNames[i], pExtensionProperties[i].extensionName,
		VK_MAX_EXTENSION_NAME_SIZE);
	}
	free(pLayerProperties);
	free(pExtensionProperties);
	*pDeviceInfo = info;
	return (VK_SUCCESS);
}


void delete_DeviceInfo(struct DeviceInfo *pDeviceInfo) {
	for (uint32_t i = 0; i < pDeviceInfo->extensionCount; i++) {
		free(pDeviceInfo->ppExtensionNames[i]);
	}
	free(pDeviceInfo->ppExtensionNames);

	for (uint32_t i = 0; i < pDeviceInfo->layerCount; i++) {
		free(pDeviceInfo->ppLayerNames[i]);
	}
	free(pDeviceInfo->ppLayerNames);
}

uint32_t new_Device(VkDevice* pDevice, const struct DeviceInfo deviceInfo,
		const VkPhysicalDevice physicalDevice, const uint32_t deviceQueueIndex,
		const uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		const uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames) {
	/* check layers and extensions */
	{
		uint32_t matchnum = 0;
		findMatchingStrings((const char* const *) deviceInfo.ppExtensionNames,
				deviceInfo.extensionCount, ppEnabledExtensionNames,
				enabledExtensionCount,
				NULL, 0, &matchnum);

		if (matchnum != enabledExtensionCount) {
			errLog(FATAL, "failed to find required device extension\n");
			panic();
		}

		findMatchingStrings((const char* const *) deviceInfo.ppLayerNames,
				deviceInfo.layerCount, ppEnabledLayerNames, enabledLayerCount,
				NULL, 0, &matchnum);

		if (matchnum != enabledLayerCount) {
			errLog(FATAL, "failed to find required device layer\n");
			panic();
		}
	}

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
		const VkSwapchainKHR oldSwapChain,
		const struct SwapChainInfo swapChainInfo, const VkDevice device,
		const VkSurfaceKHR surface, const VkExtent2D extent,
		const uint32_t graphicsIndex, const uint32_t presentIndex) {
	VkSwapchainCreateInfoKHR createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = 2;
	createInfo.imageFormat = swapChainInfo.preferredFormat.format;
	createInfo.imageColorSpace = swapChainInfo.preferredFormat.colorSpace;
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

	createInfo.preTransform =
			swapChainInfo.surfaceCapabilities.currentTransform;
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

uint32_t new_SwapChainInfo(struct SwapChainInfo *pSwapChainInfo,
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface) {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
			&pSwapChainInfo->surfaceCapabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
			&pSwapChainInfo->formatCount, NULL);
	if (pSwapChainInfo->formatCount != 0) {
		pSwapChainInfo->pFormats = malloc(
				pSwapChainInfo->formatCount * sizeof(VkSurfaceFormatKHR));
		if (!pSwapChainInfo->pFormats) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			panic();
		}
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
				&pSwapChainInfo->formatCount, pSwapChainInfo->pFormats);
	} else {
		pSwapChainInfo->pFormats = NULL;
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
			&pSwapChainInfo->presentModeCount, NULL);
	if (pSwapChainInfo->presentModeCount != 0) {
		pSwapChainInfo->pPresentModes = malloc(
				pSwapChainInfo->presentModeCount * sizeof(VkPresentModeKHR));
		if (!pSwapChainInfo->pPresentModes) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			panic();
		}
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
				&pSwapChainInfo->presentModeCount,
				pSwapChainInfo->pPresentModes);
	} else {
		pSwapChainInfo->pPresentModes = NULL;
	}


	if (pSwapChainInfo->formatCount == 1
			&& pSwapChainInfo->pFormats[0].format == VK_FORMAT_UNDEFINED) {
		/* If it has no preference, use our own */
		pSwapChainInfo->preferredFormat.colorSpace =
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		pSwapChainInfo->preferredFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	} else if (pSwapChainInfo->formatCount != 0) {
		/* we default to the first one in the list */
		pSwapChainInfo->preferredFormat = pSwapChainInfo->pFormats[0];
		/* However,  we check to make sure that what we want is in there */
		for (uint32_t i = 0; i < pSwapChainInfo->formatCount; i++) {
			VkSurfaceFormatKHR availableFormat = pSwapChainInfo->pFormats[i];
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
					&& availableFormat.colorSpace
							== VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				pSwapChainInfo->preferredFormat = availableFormat;
			}
		}
	} else {
		errLog(ERROR, "no formats available\n");
		panic();
	}

	return (VK_SUCCESS);
}

void delete_SwapChainInfo(struct SwapChainInfo *pSwapChainInfo) {
	free(pSwapChainInfo->pFormats);
	free(pSwapChainInfo->pPresentModes);
}

uint32_t new_SwapChainImages(uint32_t *pImageCount, VkImage **ppSwapChainImages,
		const VkDevice device, const VkSwapchainKHR swapChain) {
	vkGetSwapchainImagesKHR(device, swapChain, pImageCount, NULL);
	VkImage* tmp = malloc((*pImageCount) * sizeof(VkImage));
	if (!tmp) {
		errLog(FATAL, "failed to get swap chain images: %s", strerror(errno));
		panic();
	} else {
		*ppSwapChainImages = tmp;
	}
	vkGetSwapchainImagesKHR(device, swapChain, pImageCount, *ppSwapChainImages);

	return (VK_SUCCESS);
}

void delete_SwapChainImages(VkImage **ppImages) {
	free(*ppImages);
	*ppImages = NULL;
}

VkImageView new_ImageView(VkDevice device, VkImage image,
		VkFormat format) {
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
	VkImageView imageView;
	VkResult ret = vkCreateImageView(device, &createInfo, NULL,
			&imageView);
	if (ret != VK_SUCCESS) {
		errLog(FATAL, "could not create image view, error code: %d",
				(uint32_t) ret);
		panic();
	}
	return (imageView);
}

void delete_ImageView(VkDevice device, VkImageView imageView) {
	vkDestroyImageView(device, imageView, NULL);
}

uint32_t new_SwapChainImageViews(VkImageView** ppImageViews,
		const VkDevice device, const VkFormat format, const uint32_t imageCount,
		const VkImage* pSwapChainImages) {
	VkImageView* tmp = malloc(imageCount * sizeof(VkImageView));
	if (!tmp) {
		errLog(FATAL, "could not create swap chain image views: %s",
				strerror(errno));
		panic();
	} else {
		*ppImageViews = tmp;
	}

	for (uint32_t i = 0; i < imageCount; i++) {
		(*ppImageViews)[i] = new_ImageView(device, pSwapChainImages[i], format);
	}

	return (VK_SUCCESS);
}

void delete_SwapChainImageViews(VkImageView** ppImageViews, uint32_t imageCount,
		const VkDevice device) {
	for (uint32_t i = 0; i < imageCount; i++) {
		delete_ImageView(device, (*ppImageViews)[i]);
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
