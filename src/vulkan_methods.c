#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <vulkan.h>

#include "constants.h"
#include "memory.h"
#include "error_handle.h"

bool extensionsAvailable(uint32_t enabledExtensionCount, const char* const* ppEnabledExtensionNames)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	VkExtensionProperties* arr = criticalMalloc(extensionCount * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, arr);

	for(int i = 0; i < enabledExtensionCount; i++)
	{
		bool found = false;
		for(int a = 0; a < extensionCount; a++)
		{
			if(strcmp(ppEnabledExtensionNames[i], arr[a].extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			free(arr);
			return false;
		}
	}
	free(arr);
	return true;
}

bool layersAvailable(uint32_t enabledLayerCount, const char* const* ppEnabledLayerNames)
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);
	VkLayerProperties* arr = criticalMalloc(layerCount * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&layerCount, arr);

	for(int i = 0; i < enabledLayerCount; i++)
	{
		bool found = false;
		for(int a = 0; a < layerCount; a++)
		{
			if(strcmp(ppEnabledLayerNames[i], arr[a].layerName) == 0)
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			free(arr);
			return false;
		}
	}
	free(arr);
	return true;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
	fprintf(stderr, "%s\n", pCallbackData->pMessage);
	return VK_FALSE;
}

VkInstance createInstance(
		uint32_t enabledExtensionCount,
		const char* const* ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char* const* ppEnabledLayerNames)
{
	// check layers
	if(!layersAvailable(enabledLayerCount, ppEnabledLayerNames)) {
		fprintf(stderr, "requested layers not available, quitting\n");
		hardExit();
	}

	// check extensions
	if(!extensionsAvailable(enabledExtensionCount, ppEnabledExtensionNames)) {
		fprintf(stderr, "requested extensions not available, quitting\n");
		hardExit();
	}

	// Create app info
	VkApplicationInfo appInfo = {0};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = APPNAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "None";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Create info
	VkInstanceCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = enabledExtensionCount;
	createInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
	createInfo.enabledLayerCount = enabledLayerCount;
	createInfo.ppEnabledLayerNames = ppEnabledLayerNames;

	VkInstance instance = {0};
	VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
	if(result != VK_SUCCESS)
	{
		printVulkanError(result);
		hardExit();
	}
	return instance;
}

void destroyInstance(VkInstance instance) {
	vkDestroyInstance(instance,NULL);
}

/**
 * Requires the debug utils extension
 */
VkDebugUtilsMessengerEXT createDebugCallback(VkInstance instance) {
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	VkDebugUtilsMessengerEXT callback = {0};

	// Returns a function pointer
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if(func == NULL)
	{
		fprintf(stderr,"failed to find extension function, quitting\n");
		hardExit();
	}
	VkResult result = func(instance, &createInfo, NULL, &callback);
	if(result != VK_SUCCESS) {
		printVulkanError(result);
		hardExit();
	}
	return callback;
}

/**
 * Requires the debug utils extension
 */
void destroyDebugCallback(VkInstance instance, VkDebugUtilsMessengerEXT callback) {
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != NULL) {
		func(instance, callback, NULL);
	}
}

VkPhysicalDevice createPhysicalDevice(VkInstance instance) {
	uint32_t deviceCount = 0;
	VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	if(res != VK_SUCCESS || deviceCount == 0)
	{
		fprintf(stderr,"no vulkan capable device found, quitting\n");
		hardExit();
	}
	VkPhysicalDevice* arr = criticalMalloc(deviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(instance, &deviceCount, arr);

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
	for(int i = 0; i < deviceCount; i++)
	{
		vkGetDeviceProperties(arr[i],&deviceProperties,NULL);
		printf();
		int32_t ret = getDeviceQueueIndex(arr[i],VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		if(ret != -1 )
		{
			selectedDevice = arr[i];
		}
	}

	if(selectedDevice == VK_NULL_HANDLE)
	{
		fprintf(stderr,"no suitable vulkan device found, quitting\n");
		hardExit();
	}

	free(arr);
	return selectedDevice;
}

int32_t getDeviceQueueIndex(VkPhysicalDevice device, VkQueueFlags bit)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	VkQueueFamilyProperties* arr = criticalMalloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, arr);
	for(int i = 0; i < queueFamilyCount; i++)
	{
		if (arr[i].queueCount > 0 && (arr[0].queueFlags & bit)) {
			free(arr);
			return i;
		}
	}
	free(arr);
	return -1;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice,
		uint32_t deviceQueueIndex,
		uint32_t enabledExtensionCount,
		const char* const* ppEnabledExtensionNames,
		uint32_t enabledLayerCount,
		const char* const* ppEnabledLayerNames)
{
	VkPhysicalDeviceFeatures deviceFeatures = {0};

	VkDeviceQueueCreateInfo queueCreateInfo = { 0 };
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
	if(res != VK_SUCCESS)
	{
		fprintf(stderr, "failed to create device, quitting\n");
		hardExit();
	}

	return device;
}

VkQueue createQueue(VkDevice device, uint32_t deviceQueueIndex) {
	VkQueue queue;
	vkGetDeviceQueue(device, deviceQueueIndex, 0, &queue);
	return queue;
}
