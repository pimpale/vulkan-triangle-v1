#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>

#include "constants.h"
#include "errors.h"
#include "utils.h"

#include "vulkan_helper.h"

char *vkstrerror(VkResult err) {
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
			return (
			    "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_"
			    "EXT");
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
	UNUSED(messageType);
	UNUSED(pUserData);

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
	}
	/* log error */
	errLog(errcode, "Vulkan validation layer: %s", pCallbackData->pMessage);
	return (VK_FALSE);
}

/* Get required extensions */
uint32_t new_RequiredInstanceExtensions(uint32_t *pEnabledExtensionCount,
					char ***pppEnabledExtensionNames) {
	/* define our own extensions */
	/* get GLFW extensions to use */
	uint32_t glfwExtensionCount = 0;
	const char **ppGlfwExtensionNames =
	    glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	*pEnabledExtensionCount = 1 + glfwExtensionCount;
	*pppEnabledExtensionNames =
	    malloc(sizeof(char *) * (*pEnabledExtensionCount));

	if (!(*pppEnabledExtensionNames)) {
		errLog(FATAL, "failed to get required extensions: %s",
		       strerror(errno));
		panic();
	}

	/* Allocate buffers for extensions */
	for (uint32_t i = 0; i < *pEnabledExtensionCount; i++) {
		(*pppEnabledExtensionNames)[i] =
		    malloc(VK_MAX_EXTENSION_NAME_SIZE);
	}
	/* Copy our extensions in  (we're malloccing everything to make it
	 * simple to deallocate at the end without worrying about what needs to
	 * be freed or not) */
	strncpy((*pppEnabledExtensionNames)[0],
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		strncpy((*pppEnabledExtensionNames)[i+1], ppGlfwExtensionNames[i],
			VK_MAX_EXTENSION_NAME_SIZE);
	}
	return (ERR_NONE);
}

void delete_RequiredInstanceExtensions(uint32_t *pEnabledExtensionCount,
				       char ***pppEnabledExtensionNames) {
	for (uint32_t i = 0; i < *pEnabledExtensionCount; i++) {
		free((*pppEnabledExtensionNames)[i]);
	}
	free(*pppEnabledExtensionNames);
}

uint32_t new_ValidationLayers(uint32_t *pLayerCount, char ***pppLayerNames) {
	*pLayerCount = 1;
	*pppLayerNames = malloc(sizeof(char *) * sizeof(*pLayerCount));
	**pppLayerNames = "VK_LAYER_KHRONOS_validation";
	return (ERR_NONE);
}

void delete_ValidationLayers(uint32_t *pLayerCount, char ***pppLayerNames) {
	UNUSED(pLayerCount);
	free(*pppLayerNames);
}

uint32_t new_RequiredDeviceExtensions(uint32_t *pEnabledExtensionCount,
				      char ***pppEnabledExtensionNames) {
	*pEnabledExtensionCount = 1;
	*pppEnabledExtensionNames =
	    malloc(sizeof(char *) * sizeof(*pEnabledExtensionCount));
	**pppEnabledExtensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	return (ERR_NONE);
}

void delete_RequiredDeviceExtensions(uint32_t *pEnabledExtensionCount,
				     char ***pppEnabledExtensionNames) {
	UNUSED(pEnabledExtensionCount);
	free(*pppEnabledExtensionNames);
}

uint32_t new_Instance(VkInstance *pInstance,
		      const uint32_t enabledExtensionCount,
		      const char *const *ppEnabledExtensionNames,
		      const uint32_t enabledLayerCount,
		      const char *const *ppEnabledLayerNames) {
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

		errLog(FATAL, "failed to create instance, error code: %s",
		       vkstrerror(result));
		panic();
	}
	return (ERR_NONE);
}

void delete_Instance(VkInstance *pInstance) {
	vkDestroyInstance(*pInstance, NULL);
}

/**
 * Requires the debug utils extension
 */

uint32_t new_DebugCallback(VkDebugUtilsMessengerEXT *pCallback,
			   const VkInstance instance) {
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
	createInfo.sType =
	    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =
	    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	/* Returns a function pointer */
	PFN_vkCreateDebugUtilsMessengerEXT func =
	    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkCreateDebugUtilsMessengerEXT");
	if (!func) {
		errLog(FATAL, "Failed to find extension function");
		panic();
	}
	VkResult result = func(instance, &createInfo, NULL, pCallback);
	if (result != VK_SUCCESS) {
		errLog(FATAL, "Failed to create debug callback, error: %s",
		       vkstrerror(result));
		panic();
	}
	return (ERR_NOTSUPPORTED);
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

uint32_t getPhysicalDevice(VkPhysicalDevice *pDevice,
			   const VkInstance instance) {
	uint32_t deviceCount = 0;
	VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	if (res != VK_SUCCESS || deviceCount == 0) {
		errLog(WARN, "no Vulkan capable device found");
		return (ERR_NOTSUPPORTED);
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
		// TODO confirm it has required properties
		vkGetPhysicalDeviceProperties(arr[i], &deviceProperties);
		uint32_t deviceQueueIndex;
		uint32_t ret = getDeviceQueueIndex(
		    &deviceQueueIndex, arr[i],
		    VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		if (ret == VK_SUCCESS) {
			selectedDevice = arr[i];
			break;
		}
	}

	if (selectedDevice == VK_NULL_HANDLE) {
		errLog(WARN, "no suitable Vulkan device found");
		return (ERR_NOTSUPPORTED);
	}
	free(arr);
	*pDevice = selectedDevice;
	return (ERR_NONE);
}

void delete_Device(VkDevice *pDevice) { vkDestroyDevice(*pDevice, NULL); }

uint32_t getDeviceQueueIndex(uint32_t *deviceQueueIndex,
			     VkPhysicalDevice device, VkQueueFlags bit) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
						 NULL);
	if (queueFamilyCount == 0) {
		errLog(WARN, "no device queues found");
		return (ERR_NOTSUPPORTED);
	}
	VkQueueFamilyProperties *arr =
	    malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	if (!arr) {
		errLog(FATAL, "Failed to get device queue index: %s",
		      strerror(errno));
		panic();
	}
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
						 arr);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (arr[i].queueCount > 0 && (arr[0].queueFlags & bit)) {
			free(arr);
			*deviceQueueIndex = i;
			return (ERR_NONE);
		}
	}
	free(arr);
	errLog(WARN, "no suitable device queue found");
	return (ERR_NOTSUPPORTED);
}

uint32_t getPresentQueueIndex(uint32_t *pPresentQueueIndex,
			      const VkPhysicalDevice physicalDevice,
			      const VkSurfaceKHR surface) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
						 &queueFamilyCount, NULL);
	if (queueFamilyCount == 0) {
		errLog(WARN, "no queues found");
		return (ERR_NOTSUPPORTED);
	}
	VkQueueFamilyProperties *arr =
	    malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	if (!arr) {
		errLog(FATAL, "Failed to allocate memory for present queue index: %s",
		       strerror(errno));
		panic();
	}
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
						 &queueFamilyCount, arr);
	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		VkBool32 surfaceSupport;
		VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(
		    physicalDevice, i, surface, &surfaceSupport);
		if (res == VK_SUCCESS && surfaceSupport) {
			*pPresentQueueIndex = i;
			free(arr);
			return (ERR_NONE);
		}
	}
	free(arr);
	return (ERR_NOTSUPPORTED);
}

uint32_t new_Device(VkDevice *pDevice, const VkPhysicalDevice physicalDevice,
		    const uint32_t deviceQueueIndex,
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

	VkResult res =
	    vkCreateDevice(physicalDevice, &createInfo, NULL, pDevice);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "failed to create device, error: %s",
		       vkstrerror(res));
		panic();
	}
	return (ERR_NONE);
}

uint32_t getQueue(VkQueue *pQueue, const VkDevice device,
		  const uint32_t deviceQueueIndex) {
	vkGetDeviceQueue(device, deviceQueueIndex, 0, pQueue);
	return (ERR_NONE);
}

uint32_t new_SwapChain(VkSwapchainKHR *pSwapChain,
		       uint32_t *pSwapChainImageCount,
		       const VkSwapchainKHR oldSwapChain,
		       const VkSurfaceFormatKHR surfaceFormat,
		       const VkPhysicalDevice physicalDevice,
		       const VkDevice device, const VkSurfaceKHR surface,
		       const VkExtent2D extent, const uint32_t graphicsIndex,
		       const uint32_t presentIndex) {
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
						  &capabilities);

	*pSwapChainImageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 &&
	    *pSwapChainImageCount > capabilities.maxImageCount) {
		*pSwapChainImageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = *pSwapChainImageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = {graphicsIndex, presentIndex};
	if (graphicsIndex != presentIndex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;  /* Optional */
		createInfo.pQueueFamilyIndices = NULL; /* Optional */
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	/* guaranteed to be available */
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = oldSwapChain;
	VkResult res =
	    vkCreateSwapchainKHR(device, &createInfo, NULL, pSwapChain);
	if (res != VK_SUCCESS) {
		errLog(ERROR, "failed to create swap chain, error code: %s",
		       vkstrerror(res));
		panic();
	}
	return (ERR_NONE);
}

void delete_SwapChain(VkSwapchainKHR *pSwapChain, const VkDevice device) {
	vkDestroySwapchainKHR(device, *pSwapChain, NULL);
}

uint32_t getPreferredSurfaceFormat(VkSurfaceFormatKHR *pSurfaceFormat,
				   const VkPhysicalDevice physicalDevice,
				   const VkSurfaceKHR surface) {
	uint32_t formatCount;
	VkSurfaceFormatKHR *pSurfaceFormats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
					     &formatCount, NULL);
	if (formatCount != 0) {
		pSurfaceFormats =
		    malloc(formatCount * sizeof(VkSurfaceFormatKHR));
		if (!pSurfaceFormats) {
			errLog(FATAL, "could not get preferred format: %s",
			       strerror(errno));
			panic();
		}
		vkGetPhysicalDeviceSurfaceFormatsKHR(
		    physicalDevice, surface, &formatCount, pSurfaceFormats);
	} else {
		pSurfaceFormats = NULL;
		return (ERR_NOTSUPPORTED);
	}

	VkSurfaceFormatKHR preferredFormat;
	if (formatCount == 1 &&
	    pSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		/* If it has no preference, use our own */
		preferredFormat = pSurfaceFormats[0];
	} else if (formatCount != 0) {
		/* we default to the first one in the list */
		preferredFormat = pSurfaceFormats[0];
		/* However,  we check to make sure that what we want is in there
		 */
		for (uint32_t i = 0; i < formatCount; i++) {
			VkSurfaceFormatKHR availableFormat = pSurfaceFormats[i];
			if (availableFormat.format ==
				VK_FORMAT_B8G8R8A8_UNORM &&
			    availableFormat.colorSpace ==
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				preferredFormat = availableFormat;
			}
		}
	} else {
		errLog(ERROR, "no formats available");
		panic();
	}

	free(pSurfaceFormats);

	*pSurfaceFormat = preferredFormat;
	return (VK_SUCCESS);
}

uint32_t new_SwapChainImages(VkImage **ppSwapChainImages, uint32_t *pImageCount,
			     const VkDevice device,
			     const VkSwapchainKHR swapChain) {
	vkGetSwapchainImagesKHR(device, swapChain, pImageCount, NULL);

	if (pImageCount == 0) {
		errLog(WARN, "cannot create zero images");
		return (ERR_UNSAFE);
	}

	*ppSwapChainImages = malloc((*pImageCount) * sizeof(VkImage));
	if (!*ppSwapChainImages) {
		errLog(FATAL, "failed to get swap chain images: %s",
		       strerror(errno));
		panic();
	}
	VkResult res = vkGetSwapchainImagesKHR(device, swapChain, pImageCount,
					       *ppSwapChainImages);
	if (res != VK_SUCCESS) {
		errLog(WARN, "failed to get swap chain images, error: %s",
		       vkstrerror(res));
		return (ERR_UNKNOWN);
	} else {
		return (ERR_NONE);
	}
}

void delete_SwapChainImages(VkImage **ppImages) {
	free(*ppImages);
	*ppImages = NULL;
}

uint32_t new_ImageView(VkImageView *pImageView, const VkDevice device,
		       const VkImage image, const VkFormat format) {
	VkImageViewCreateInfo createInfo = {0};
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
	VkResult ret = vkCreateImageView(device, &createInfo, NULL, pImageView);
	if (ret != VK_SUCCESS) {
		errLog(FATAL, "failed to create image view, error: %s",
		       vkstrerror(ret));
		panic();
	}
	return (ERR_NONE);
}

void delete_ImageView(VkImageView *pImageView, VkDevice device) {
	vkDestroyImageView(device, *pImageView, NULL);
}

uint32_t new_SwapChainImageViews(VkImageView **ppImageViews,
				 const VkDevice device, const VkFormat format,
				 const uint32_t imageCount,
				 const VkImage *pSwapChainImages) {
	if (imageCount == 0) {
		errLog(WARN, "cannot create zero image views");
		return (ERR_UNSAFE);
	}
	VkImageView *pImageViews = malloc(imageCount * sizeof(VkImageView));
	if (!pImageViews) {
		errLog(FATAL, "could not create swap chain image views: %s",
		       strerror(errno));
		panic();
	}

	for (uint32_t i = 0; i < imageCount; i++) {
		uint32_t ret = new_ImageView(&(pImageViews[i]), device,
					     pSwapChainImages[i], format);
		if (ret != VK_SUCCESS) {
			errLog(FATAL,
			       "failed to create image view, error: %s",
			       vkstrerror(ret));
			panic();
		}
	}

	*ppImageViews = pImageViews;
	return (ERR_NONE);
}

void delete_SwapChainImageViews(VkImageView **ppImageViews, uint32_t imageCount,
				const VkDevice device) {
	for (uint32_t i = 0; i < imageCount; i++) {
		delete_ImageView(&((*ppImageViews)[i]), device);
	}
	free(*ppImageViews);
	*ppImageViews = NULL;
}

uint32_t new_ShaderModule(VkShaderModule *pShaderModule, const VkDevice device,
			  const uint32_t codeSize, const uint32_t *pCode) {
	VkShaderModuleCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeSize;
	createInfo.pCode = pCode;
	VkResult res =
	    vkCreateShaderModule(device, &createInfo, NULL, pShaderModule);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create shader module");
	}
	return (ERR_NONE);
}

uint32_t new_ShaderModuleFromFile(VkShaderModule *pShaderModule,
				  const VkDevice device, char *filename) {
	uint32_t *shaderFileContents;
	uint32_t shaderFileLength;
	readShaderFile(filename, &shaderFileLength, &shaderFileContents);
	uint32_t retval = new_ShaderModule(pShaderModule, device, shaderFileLength,
			 shaderFileContents);
	free(shaderFileContents);
	return (retval);
}

void delete_ShaderModule(VkShaderModule *pShaderModule, const VkDevice device) {
	vkDestroyShaderModule(device, *pShaderModule, NULL);
}

uint32_t new_VertexDisplayRenderPass(VkRenderPass *pRenderPass, const VkDevice device,
			const VkFormat swapChainImageFormat) {
	VkAttachmentDescription colorAttachment = {0};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {0};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {0};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {0};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = {0};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
				   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult res =
	    vkCreateRenderPass(device, &renderPassInfo, NULL, pRenderPass);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create render pass, error: %s",
		       vkstrerror(res));
		panic();
	}
	return (ERR_NONE);
}

void delete_RenderPass(VkRenderPass *pRenderPass, const VkDevice device) {
	vkDestroyRenderPass(device, *pRenderPass, NULL);
}

uint32_t new_VertexDisplayPipelineLayout(VkPipelineLayout *pPipelineLayout,
			    const VkDevice device) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
	pipelineLayoutInfo.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	VkResult res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL,
					      pPipelineLayout);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create pipeline layout with error: %s",
		       vkstrerror(res));
		panic();
	}
	return (ERR_NONE);
}

void delete_PipelineLayout(VkPipelineLayout *pPipelineLayout,
			   const VkDevice device) {
	vkDestroyPipelineLayout(device, *pPipelineLayout, NULL);
}

uint32_t new_VertexDisplayPipeline(VkPipeline *pGraphicsPipeline,
			      const VkDevice device,
			      const VkShaderModule vertShaderModule,
			      const VkShaderModule fragShaderModule,
			      const VkExtent2D extent,
			      const VkRenderPass renderPass,
			      const VkPipelineLayout pipelineLayout) {
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
	vertShaderStageInfo.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
	fragShaderStageInfo.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[2] = {vertShaderStageInfo,
							   fragShaderStageInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
	vertexInputInfo.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
	inputAssembly.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {0};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {0};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportState = {0};
	viewportState.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {0};
	rasterizer.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {0};
	multisampling.sType =
	    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
	colorBlendAttachment.colorWriteMask =
	    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {0};
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

	VkGraphicsPipelineCreateInfo pipelineInfo = {0};
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
		errLog(FATAL, "failed to create graphics pipeline!");
		panic();
	}
	return (ERR_NONE);
}

void delete_Pipeline(VkPipeline *pPipeline, const VkDevice device) {
	vkDestroyPipeline(device, *pPipeline, NULL);
}

uint32_t new_Framebuffer(VkFramebuffer *pFramebuffer, const VkDevice device,
			 const VkRenderPass renderPass,
			 const VkImageView imageView,
			 const VkExtent2D swapChainExtent) {
	VkFramebufferCreateInfo framebufferInfo = {0};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = (VkImageView[]){imageView};
	framebufferInfo.width = swapChainExtent.width;
	framebufferInfo.height = swapChainExtent.height;
	framebufferInfo.layers = 1;
	VkResult res =
	    vkCreateFramebuffer(device, &framebufferInfo, NULL, pFramebuffer);
	if (res == VK_SUCCESS) {
		return (ERR_NONE);
	} else {
		errLog(WARN, "failed to create framebuffers: %s",
		       vkstrerror(res));
		return (ERR_UNKNOWN);
	}
}

void delete_Framebuffer(VkFramebuffer *pFramebuffer, VkDevice device) {
	vkDestroyFramebuffer(device, *pFramebuffer, NULL);
}

uint32_t new_SwapChainFramebuffers(VkFramebuffer **ppFramebuffers,
				   const VkDevice device,
				   const VkRenderPass renderPass,
				   const VkExtent2D swapChainExtent,
				   const uint32_t imageCount,
				   const VkImageView *pSwapChainImageViews) {
	VkFramebuffer *tmp = malloc(imageCount * sizeof(VkFramebuffer));
	if (!tmp) {
		errLog(FATAL, "could not create framebuffers: %s",
		       strerror(errno));
		panic();
	}

	for (uint32_t i = 0; i < imageCount; i++) {
		uint32_t res =
		    new_Framebuffer(&(tmp[i]), device, renderPass,
				    pSwapChainImageViews[i], swapChainExtent);
		if (res != VK_SUCCESS) {
			errLog(ERROR,
			       "could not create framebuffer, error code: %d",
			       res);
			return (res);
		}
	}

	*ppFramebuffers = tmp;
	return (ERR_NONE);
}

void delete_SwapChainFramebuffers(VkFramebuffer **ppFramebuffers,
				  const uint32_t imageCount,
				  const VkDevice device) {
	for (uint32_t i = 0; i < imageCount; i++) {
		delete_Framebuffer(&((*ppFramebuffers)[i]), device);
	}
	free(*ppFramebuffers);
	*ppFramebuffers = NULL;
}

uint32_t new_CommandPool(VkCommandPool *pCommandPool, const VkDevice device,
			 const uint32_t queueFamilyIndex) {
	VkCommandPoolCreateInfo poolInfo = {0};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = 0;
	VkResult ret =
	    vkCreateCommandPool(device, &poolInfo, NULL, pCommandPool);
	return (ret);
}

void delete_CommandPool(VkCommandPool *pCommandPool, const VkDevice device) {
	vkDestroyCommandPool(device, *pCommandPool, NULL);
}

uint32_t new_GraphicsCommandBuffers(
    VkCommandBuffer **ppCommandBuffers, const VkDevice device,
    const VkRenderPass renderPass, const VkPipeline graphicsPipeline,
    const VkCommandPool commandPool, const VkExtent2D swapChainExtent,
    const uint32_t swapChainFramebufferCount,
    const VkFramebuffer *pSwapChainFramebuffers) {
	VkCommandBufferAllocateInfo allocInfo = {0};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = swapChainFramebufferCount;

	VkCommandBuffer *pCommandBuffers =
	    malloc(swapChainFramebufferCount * sizeof(VkCommandBuffer));
	if (!pCommandBuffers) {
		errLog(FATAL, "failed to create graphics command buffers: %s",
		       strerror(errno));
		panic();
	}

	VkResult ret =
	    vkAllocateCommandBuffers(device, &allocInfo, pCommandBuffers);
	if (ret != VK_SUCCESS) {
		errLog(
		    FATAL,
		    "failed to create graphics command buffers, error code: %s",
		    vkstrerror(ret));
		panic();
	}

	for (size_t i = 0; i < swapChainFramebufferCount; i++) {
		VkCommandBufferBeginInfo beginInfo = {0};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(pCommandBuffers[i], &beginInfo) !=
		    VK_SUCCESS) {
			errLog(FATAL,
			       "Failed to record into graphics command buffer");
			panic();
		}

		VkRenderPassBeginInfo renderPassInfo = {0};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = pSwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor;
		clearColor.color.float32[0] = 0.0f;
		clearColor.color.float32[1] = 0.0f;
		clearColor.color.float32[2] = 0.0f;
		clearColor.color.float32[3] = 0.0f;

		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(pCommandBuffers[i], &renderPassInfo,
				     VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(pCommandBuffers[i],
				  VK_PIPELINE_BIND_POINT_GRAPHICS,
				  graphicsPipeline);

		vkCmdDraw(pCommandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(pCommandBuffers[i]);

		VkResult ret = vkEndCommandBuffer(pCommandBuffers[i]);
		if (ret != VK_SUCCESS) {
			errLog(
			    FATAL,
			    "failed to record command buffer, error: %s",
			    vkstrerror(ret));
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

uint32_t new_Semaphore(VkSemaphore *pSemaphore, const VkDevice device) {
	VkSemaphoreCreateInfo semaphoreInfo = {0};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	return (vkCreateSemaphore(device, &semaphoreInfo, NULL, pSemaphore));
}

void delete_Semaphore(VkSemaphore *pSemaphore, const VkDevice device) {
	vkDestroySemaphore(device, *pSemaphore, NULL);
}

uint32_t new_Semaphores(VkSemaphore **ppSemaphores,
			const uint32_t semaphoreCount, const VkDevice device) {
	if (semaphoreCount == 0) {
		errLog(WARN, "failed to create semaphores: %s",
		       "Failed to allocate 0 bytes of memory");
	}
	*ppSemaphores = malloc(semaphoreCount * sizeof(VkSemaphore));
	if (*ppSemaphores == NULL) {
		errLog(FATAL, "Failed to create semaphores: %s",
		       strerror(errno));
	}

	for (uint32_t i = 0; i < semaphoreCount; i++) {
		new_Semaphore(&(*ppSemaphores)[i], device);
	}
	return (ERR_NONE);
}

void delete_Semaphores(VkSemaphore **ppSemaphores,
		       const uint32_t semaphoreCount, const VkDevice device) {
	for (uint32_t i = 0; i < semaphoreCount; i++) {
		delete_Semaphore(&((*ppSemaphores)[i]), device);
	}
	free(*ppSemaphores);
}

uint32_t new_Fence(VkFence *pFence, const VkDevice device) {
	VkFenceCreateInfo fenceInfo = {0};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	return (vkCreateFence(device, &fenceInfo, NULL, pFence));
}

void delete_Fence(VkFence *pFence, const VkDevice device) {
	vkDestroyFence(device, *pFence, NULL);
}

uint32_t new_Fences(VkFence **ppFences, const uint32_t fenceCount,
		    const VkDevice device) {
	if (fenceCount == 0) {
		errLog(WARN, "cannot allocate 0 bytes of memory");
		return (ERR_UNSAFE);
	}
	*ppFences = malloc(fenceCount * sizeof(VkDevice));
	if (!*ppFences) {
		errLog(FATAL, "failed to create memory fence; %s",
		       strerror(errno));
		panic();
	}

	for (uint32_t i = 0; i < fenceCount; i++) {
		new_Fence(&(*ppFences)[i], device);
	}
	return (ERR_NONE);
}

void delete_Fences(VkFence **ppFences, const uint32_t fenceCount,
		   const VkDevice device) {
	for (uint32_t i = 0; i < fenceCount; i++) {
		delete_Fence(&(*ppFences)[i], device);
	}
	free(*ppFences);
}

uint32_t drawFrame(uint32_t *pCurrentFrame, const uint32_t maxFramesInFlight,
		   const VkDevice device, const VkSwapchainKHR swapChain,
		   const VkCommandBuffer *pCommandBuffers,
		   const VkFence *pInFlightFences,
		   const VkSemaphore *pImageAvailableSemaphores,
		   const VkSemaphore *pRenderFinishedSemaphores,
		   const VkQueue graphicsQueue, const VkQueue presentQueue) {
	VkResult fenceWait = vkWaitForFences(
	    device, 1, &pInFlightFences[*pCurrentFrame], VK_TRUE, UINT64_MAX);
	if (fenceWait != VK_SUCCESS) {
		errLog(FATAL,
		       "failed to wait for fence while drawing frame: %s",
		       vkstrerror(fenceWait));
		panic();
	}
	VkResult fenceResetResult =
	    vkResetFences(device, 1, &pInFlightFences[*pCurrentFrame]);
	if (fenceResetResult != VK_SUCCESS) {
		errLog(FATAL, "failed to reset fence while drawing frame: %s",
		       vkstrerror(fenceResetResult));
	}
	uint32_t imageIndex;
	VkResult nextImageResult =
	    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
				  pImageAvailableSemaphores[*pCurrentFrame],
				  VK_NULL_HANDLE, &imageIndex);

	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
		return (ERR_OUTOFDATE);
	} else if (nextImageResult != VK_SUCCESS) {
		errLog(FATAL, "failed to get next frame: %s",
		       vkstrerror(nextImageResult));
		panic();
	}

	VkSubmitInfo submitInfo = {0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {
	    pImageAvailableSemaphores[*pCurrentFrame]};
	VkPipelineStageFlags waitStages[] = {
	    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &pCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = {
	    pRenderFinishedSemaphores[*pCurrentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult queueSubmitResult = vkQueueSubmit(
	    graphicsQueue, 1, &submitInfo, pInFlightFences[*pCurrentFrame]);
	if (queueSubmitResult != VK_SUCCESS) {
		errLog(FATAL, "failed to submit queue: %s",
		       vkstrerror(queueSubmitResult));
		panic();
	}

	VkPresentInfoKHR presentInfo = {0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(presentQueue, &presentInfo);
	*pCurrentFrame = (*pCurrentFrame + 1) % maxFramesInFlight;

	return (ERR_NONE);
}

void delete_Surface(VkSurfaceKHR *pSurface, const VkInstance instance) {
	vkDestroySurfaceKHR(instance, *pSurface, NULL);
}

uint32_t getWindowExtent(VkExtent2D *pExtent, GLFWwindow *pWindow) {
	int width;
	int height;
	glfwGetFramebufferSize(pWindow, &width, &height);
	pExtent->width = (uint32_t)width;
	pExtent->height = (uint32_t)height;
	return (ERR_NONE);
}

uint32_t new_GLFWwindow(GLFWwindow **ppGLFWwindow) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	*ppGLFWwindow =
	    glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APPNAME, NULL, NULL);
	if (*ppGLFWwindow == NULL) {
		return (ERR_UNKNOWN);
	}
	return (ERR_NONE);
}

uint32_t new_SurfaceFromGLFW(VkSurfaceKHR *pSurface, GLFWwindow *pWindow,
		     const VkInstance instance) {
	VkResult res =
	    glfwCreateWindowSurface(instance, pWindow, NULL, pSurface);
	if (res != VK_SUCCESS) {
		errLog(FATAL, "failed to create surface, quitting");
		panic();
	}
	return (ERR_NONE);
}
