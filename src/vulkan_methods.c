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

VkInstance new_Instance(struct InstanceInfo instanceInfo,
		uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char *const *ppEnabledLayerNames) {

	/* check layers and extensions */
	{
		uint32_t matchnum = 0;
		findMatchingStrings((const char* const *) instanceInfo.ppExtensionNames,
				instanceInfo.extensionCount, ppEnabledExtensionNames,
				enabledExtensionCount,
				NULL, 0, &matchnum);

		if (matchnum != enabledExtensionCount) {
			errLog(FATAL, "failed to find required extension\n");
			panic();
		}

		findMatchingStrings((const char* const *) instanceInfo.ppLayerNames,
				instanceInfo.layerCount, ppEnabledLayerNames, enabledLayerCount,
				NULL, 0, &matchnum);

		if (matchnum != enabledLayerCount) {
			errLog(FATAL, "failed to find required layer\n");
			panic();
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

	VkInstance instance = {0};
	VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
	if (result != VK_SUCCESS) {
		errLog(FATAL, "Failed to create instance, error code: %d",
				(uint32_t) result);
		panic();
	}
	return (instance);
}

void delete_Instance(VkInstance instance) { vkDestroyInstance(instance, NULL); }

/**
 * Requires the debug utils extension
 */
VkDebugUtilsMessengerEXT new_DebugCallback(VkInstance instance) {
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	VkDebugUtilsMessengerEXT callback = {0};

	/* Returns a function pointer */
	PFN_vkCreateDebugUtilsMessengerEXT func =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					instance, "vkCreateDebugUtilsMessengerEXT");
	if (!func) {
		errLog(FATAL, "failed to find extension function\n");
		panic();
	}
	VkResult result = func(instance, &createInfo, NULL, &callback);
	if (result != VK_SUCCESS) {
		errLog(ERROR, "Failed to create debug callback, error code: %d",
				(uint32_t) result);
		panic();
	}
	return (callback);
}

/**
 * Requires the debug utils extension
 */
void delete_DebugCallback(VkInstance instance,
		VkDebugUtilsMessengerEXT callback) {
	PFN_vkDestroyDebugUtilsMessengerEXT func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != NULL) {
		func(instance, callback, NULL);
	}
}

VkPhysicalDevice getPhysicalDevice(VkInstance instance) {
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
		int32_t ret = getDeviceQueueIndex(arr[i], VK_QUEUE_GRAPHICS_BIT |
				VK_QUEUE_COMPUTE_BIT);
		if (ret != -1) {
			selectedDevice = arr[i];
		}
	}

	if (selectedDevice == VK_NULL_HANDLE) {
		errLog(ERROR, "no suitable Vulkan device found\n");
		panic();
	}

	free(arr);
	return (selectedDevice);
}

void delete_Device(VkDevice device) { vkDestroyDevice(device, NULL); }

int32_t getDeviceQueueIndex(VkPhysicalDevice device, VkQueueFlags bit) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	VkQueueFamilyProperties *arr =
			malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	if (!arr) {
		errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
		panic();
	}
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, arr);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (arr[i].queueCount > 0 && (arr[0].queueFlags & bit)) {
			free(arr);
			return (i);
		}
	}
	free(arr);
	return (-1);
}

int32_t getPresentQueueIndex(VkPhysicalDevice device, VkSurfaceKHR surface) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	VkQueueFamilyProperties *arr =
			malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	if (!arr) {
		errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
		panic();
	}
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, arr);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		VkBool32 surfaceSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);
		if (surfaceSupport) {
			return (i);
		}
	}
	free(arr);
	return (-1);
}

struct DeviceIndices new_DeviceIndices(VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface) {
	struct DeviceIndices deviceIndices = { 0 };

	int32_t tmp = getDeviceQueueIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
	if (tmp != -1) {
		deviceIndices.hasGraphics = true;
		deviceIndices.graphicsIndex = (uint32_t) tmp;
	} else {
		deviceIndices.hasGraphics = false;
	}

	tmp = getDeviceQueueIndex(physicalDevice, VK_QUEUE_COMPUTE_BIT);
	if (tmp != -1) {
		deviceIndices.hasCompute = true;
		deviceIndices.computeIndex = (uint32_t) tmp;
	} else {
		deviceIndices.hasCompute = false;
	}

	deviceIndices.hasPresent = false;
	if (surface != VK_NULL_HANDLE) {
		tmp = getPresentQueueIndex(physicalDevice, surface);
		if (tmp != -1) {
			deviceIndices.hasPresent = true;
			deviceIndices.presentIndex = (uint32_t) tmp;
		}
	}
	return (deviceIndices);
}

struct InstanceInfo new_InstanceInfo() {
	struct InstanceInfo info;
	vkEnumerateInstanceLayerProperties(&info.layerCount, NULL);
	vkEnumerateInstanceExtensionProperties(NULL, &info.extensionCount, NULL);


	/* alloc number dependent info */
	info.ppLayerNames = malloc(info.layerCount * sizeof(char*));
	info.ppExtensionNames = malloc(info.extensionCount * sizeof(char*));
	VkLayerProperties* pLayerProperties = malloc(
			info.layerCount * sizeof(VkLayerProperties));
	VkExtensionProperties* pExtensionProperties = malloc(
			info.extensionCount * sizeof(VkExtensionProperties));

	/* enumerate */
	vkEnumerateInstanceLayerProperties(&info.layerCount, pLayerProperties);
	vkEnumerateInstanceExtensionProperties(NULL, &info.extensionCount,
			pExtensionProperties);

	/* check if not null */
	if (!info.ppExtensionNames || !info.ppLayerNames || !pLayerProperties
			|| !pExtensionProperties) {
		errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
		panic();
	}

	/* copy names to info */
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
	return (info);
}


void delete_InstanceInfo(struct InstanceInfo instanceInfo) {
	for (uint32_t i = 0; i < instanceInfo.extensionCount; i++) {
		free(instanceInfo.ppExtensionNames[i]);
	}
	free(instanceInfo.ppExtensionNames);

	for (uint32_t i = 0; i < instanceInfo.layerCount; i++) {
		free(instanceInfo.ppLayerNames[i]);
	}
	free(instanceInfo.ppLayerNames);
}


struct DeviceInfo new_DeviceInfo(VkPhysicalDevice physicalDevice)
{
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
	return (info);
}


void delete_DeviceInfo(struct DeviceInfo deviceInfo) {
	for (uint32_t i = 0; i < deviceInfo.extensionCount; i++) {
		free(deviceInfo.ppExtensionNames[i]);
	}
	free(deviceInfo.ppExtensionNames);

	for (uint32_t i = 0; i < deviceInfo.layerCount; i++) {
		free(deviceInfo.ppLayerNames[i]);
	}
	free(deviceInfo.ppLayerNames);
}

VkDevice new_Device(struct DeviceInfo deviceInfo,
		VkPhysicalDevice physicalDevice,
		uint32_t deviceQueueIndex,
		uint32_t enabledExtensionCount,
		const char *const *ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
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

	VkDevice device;
	VkResult res = vkCreateDevice(physicalDevice, &createInfo, NULL, &device);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "Failed to create device, error code: %d",
				(uint32_t) res);
		panic();
	}

	return (device);
}

VkQueue getQueue(VkDevice device, uint32_t deviceQueueIndex) {
	VkQueue queue;
	vkGetDeviceQueue(device, deviceQueueIndex, 0, &queue);
	return (queue);
}

VkSwapchainKHR new_SwapChain(VkSwapchainKHR oldSwapChain,
		struct SwapChainInfo swapChainInfo,
		VkDevice device,
		VkSurfaceKHR surface, VkExtent2D extent,
		struct DeviceIndices deviceIndices) {

	VkSwapchainKHR swapChain;
	VkSwapchainCreateInfoKHR createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = 2;
	createInfo.imageFormat = swapChainInfo.preferredFormat.format;
	createInfo.imageColorSpace = swapChainInfo.preferredFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (!deviceIndices.hasGraphics || !deviceIndices.hasPresent) {
		errLog(FATAL, "Invalid device to create swap chain\n");
		panic();
	}

	uint32_t queueFamilyIndices[] = { deviceIndices.graphicsIndex,
			deviceIndices.presentIndex };
	if (deviceIndices.graphicsIndex != deviceIndices.presentIndex) {
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
	VkResult res = vkCreateSwapchainKHR(device, &createInfo, NULL, &swapChain);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "Failed to create swap chain, error code: %d",
				(uint32_t) res);
		panic();
	}

	return (swapChain);
}

void delete_SwapChain(VkDevice device, VkSwapchainKHR swapChain) {
	vkDestroySwapchainKHR(device, swapChain, NULL);
}

struct SwapChainInfo new_SwapChainInfo(VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface) {
	struct SwapChainInfo swapChainInfo;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
			&swapChainInfo.surfaceCapabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
			&swapChainInfo.formatCount, NULL);
	if (swapChainInfo.formatCount != 0) {
		swapChainInfo.pFormats = malloc(
				swapChainInfo.formatCount * sizeof(VkSurfaceFormatKHR));
		if (!swapChainInfo.pFormats) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			panic();
		}
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
				&swapChainInfo.formatCount, swapChainInfo.pFormats);
	} else {
		swapChainInfo.pFormats = NULL;
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
			&swapChainInfo.presentModeCount, NULL);
	if (swapChainInfo.presentModeCount != 0) {
		swapChainInfo.pPresentModes = malloc(
				swapChainInfo.presentModeCount * sizeof(VkPresentModeKHR));
		if (!swapChainInfo.pPresentModes) {
			errLog(FATAL, "failed to allocate memory: %s", strerror(errno));
			panic();
		}
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
				&swapChainInfo.presentModeCount, swapChainInfo.pPresentModes);
	} else {
		swapChainInfo.pPresentModes = NULL;
	}


	if (swapChainInfo.formatCount == 1
			&& swapChainInfo.pFormats[0].format == VK_FORMAT_UNDEFINED) {
		/* If it has no preference, use our own */
		swapChainInfo.preferredFormat.colorSpace =
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		swapChainInfo.preferredFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	} else if (swapChainInfo.formatCount != 0) {
		/* we default to the first one in the list */
		swapChainInfo.preferredFormat = swapChainInfo.pFormats[0];
		/* However,  we check to make sure that what we want is in there */
		for (uint32_t i = 0; i < swapChainInfo.formatCount; i++) {
			VkSurfaceFormatKHR availableFormat = swapChainInfo.pFormats[i];
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
					&& availableFormat.colorSpace
							== VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				swapChainInfo.preferredFormat = availableFormat;
			}
		}
	} else {
		errLog(ERROR, "no formats available\n");
		panic();
	}


	return (swapChainInfo);
}

void delete_SwapChainInfo(struct SwapChainInfo swapChainInfo) {
	free(swapChainInfo.pFormats);
	free(swapChainInfo.pPresentModes);
}

void new_SwapChainImages(VkDevice device, VkSwapchainKHR swapChain,
		uint32_t *pImageCount, VkImage **ppSwapChainImages) {
	vkGetSwapchainImagesKHR(device, swapChain, pImageCount, NULL);
	VkImage* tmp = malloc((*pImageCount) * sizeof(VkImage));
	if (!tmp) {
		errLog(FATAL, "failed to get swap chain images: %s", strerror(errno));
		panic();
	} else {
		*ppSwapChainImages = tmp;
	}
	vkGetSwapchainImagesKHR(device, swapChain, pImageCount, *ppSwapChainImages);
}

void delete_SwapChainImages(VkImage *pImages) {
	free(pImages);
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

void new_SwapChainImageViews(VkDevice device, VkFormat format,
		uint32_t imageCount, VkImage* pSwapChainImages, VkImageView** ppImageViews) {
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
}

void delete_SwapChainImageViews(VkDevice device, uint32_t imageCount,
		VkImageView* pImageViews) {
	for (uint32_t i = 0; i < imageCount; i++) {
		delete_ImageView(device, pImageViews[i]);
	}
	free(pImageViews);
}

VkShaderModule new_ShaderModule(VkDevice device, uint32_t codeSize,
		uint32_t* pCode) {
	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeSize;
	createInfo.pCode = pCode;
	VkShaderModule shaderModule;
	VkResult res = vkCreateShaderModule(device, &createInfo, NULL,
			&shaderModule);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create shader module");
	}
	return (shaderModule);
}

void delete_ShaderModule(VkDevice device, VkShaderModule shaderModule) {
	vkDestroyShaderModule(device, shaderModule, NULL);
}

VkPipeline new_GraphicsPipeline(VkDevice device,
		VkShaderModule vertShaderModule, VkShaderModule fragShaderModule,
		VkExtent2D extent, VkRenderPass renderPass) {
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = { };
	vertShaderStageInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = { };
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

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VkPipelineLayout pipelineLayout;
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL,
			&pipelineLayout) != VK_SUCCESS) {
		errLog(FATAL, "failed to create pipeline layout\n");
		panic();
	}

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

	VkPipeline graphicsPipeline;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
			NULL, &graphicsPipeline) != VK_SUCCESS) {
		errLog(FATAL, "failed to create graphics pipeline!\n");
		panic();
	}

	return (graphicsPipeline);
}

