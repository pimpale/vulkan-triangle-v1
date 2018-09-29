#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan.h>
#define GLFW_DEFINE_VULKAN
#include <glfw3.h>

#include "error_methods.h"
#include "glfw_methods.h"
#include "util_methods.h"
#include "vulkan_methods.h"

int main(void) {
  glfwInit();

  // define our own extensions
  // get glfw extensions to use
  uint32_t glfwExtensionCount = 0;
  const char **ppGlfwExtensionNames =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  uint32_t extensionCount = 1 + glfwExtensionCount;
  const char **ppExtensionNames = malloc(sizeof(char *) * extensionCount);
  if (!ppExtensionNames) {
    printError(errno);
    hardExit();
  }

  concatArray((char *[]){VK_EXT_DEBUG_UTILS_EXTENSION_NAME},
			  (uint32_t)(1 * sizeof(char **)),
			  (uint32_t)(extensionCount * sizeof(char **)),
              ppGlfwExtensionNames,
              ppExtensionNames,
              (uint32_t)(glfwExtensionCount * sizeof(char **)));

  // Initialize layers to use
  uint32_t layerCount = 1;
  const char *ppLayerNames[] = {"VK_LAYER_LUNARG_standard_validation"};

  // Device extensions to be used
  uint32_t deviceExtensionCount = 1;
  const char *ppDeviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  // Create instance
  VkInstance instance = createInstance(extensionCount, ppExtensionNames,
                                       layerCount, ppLayerNames);
  VkDebugUtilsMessengerEXT callback = createDebugCallback(instance);

  VkPhysicalDevice physicalDevice = createPhysicalDevice(instance);

  // Create window and surface
  GLFWwindow *pWindow = createGlfwWindow();
  VkSurfaceKHR surface = createSurface(pWindow, instance);

  // find queue on graphics device
  int32_t graphicsQueueIndex =
      getDeviceQueueIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
  int32_t presentQueueIndex = getPresentQueueIndex(physicalDevice, surface);
  if (graphicsQueueIndex == -1 || presentQueueIndex == -1) {
    fprintf(stderr, "found no suitable queue on device, quitting\n");
    hardExit();
  }
  VkDevice device = createLogicalDevice(
      physicalDevice, (uint32_t)graphicsQueueIndex, deviceExtensionCount,
      ppDeviceExtensionNames, layerCount, ppLayerNames);

  // create queues
  VkQueue graphicsQueue = createQueue(device, (uint32_t)graphicsQueueIndex);
  VkQueue presentQueue = createQueue(device, (uint32_t)presentQueueIndex);

  while (!glfwWindowShouldClose(pWindow)) {
    glfwPollEvents();
  }

  puts("quitting");

  destroyDevice(device);
  destroyDebugCallback(instance, callback);
  destroyInstance(instance);
  free(ppExtensionNames);
  glfwTerminate();
  return EXIT_SUCCESS;
}
