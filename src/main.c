#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "constants.h"
#include "errors.h"
#include "utils.h"
#include "vulkan_utils.h"

int main(void) {
  glfwInit();

  const uint32_t validationLayerCount = 1;
  const char *ppValidationLayerNames[1] = {"VK_LAYER_KHRONOS_validation"};

  /* Create instance */
  VkInstance instance;
  new_Instance(&instance, validationLayerCount, ppValidationLayerNames, 0, NULL,
               true, true);

  /* Enable vulkan logging to stdout */
  VkDebugUtilsMessengerEXT callback;
  new_DebugCallback(&callback, instance);

  /* get physical device */
  VkPhysicalDevice physicalDevice;
  getPhysicalDevice(&physicalDevice, instance);

  /* Create window and surface */
  GLFWwindow *pWindow;
  new_GlfwWindow(&pWindow, APPNAME,
                 (VkExtent2D){.width = WINDOW_WIDTH, .height = WINDOW_HEIGHT});
  VkSurfaceKHR surface;
  new_SurfaceFromGLFW(&surface, pWindow, instance);

  /* find queues on graphics device */
  uint32_t graphicsIndex;
  uint32_t computeIndex;
  uint32_t presentIndex;
  {
    uint32_t ret1 = getDeviceQueueIndex(&graphicsIndex, physicalDevice,
                                        VK_QUEUE_GRAPHICS_BIT);
    uint32_t ret2 = getDeviceQueueIndex(&computeIndex, physicalDevice,
                                        VK_QUEUE_COMPUTE_BIT);
    uint32_t ret3 =
        getPresentQueueIndex(&presentIndex, physicalDevice, surface);
    /* Panic if indices are unavailable */
    if (ret1 != VK_SUCCESS || ret2 != VK_SUCCESS || ret3 != VK_SUCCESS) {
      LOG_ERROR(ERR_LEVEL_FATAL, "unable to acquire indices\n");
      PANIC();
    }
  }

  /* Set extent (for now just window width and height) */
  VkExtent2D swapChainExtent;
  getWindowExtent(&swapChainExtent, pWindow);

  /* we want to use swapchains to reduce tearing */
  const uint32_t deviceExtensionCount = 1;
  const char *ppDeviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  /*create device */
  VkDevice device;
  new_Device(&device, physicalDevice, graphicsIndex, deviceExtensionCount,
             ppDeviceExtensionNames);

  VkQueue graphicsQueue;
  getQueue(&graphicsQueue, device, graphicsIndex);
  VkQueue computeQueue;
  getQueue(&computeQueue, device, computeIndex);
  VkQueue presentQueue;
  getQueue(&presentQueue, device, presentIndex);

  /* We can create command buffers from the command pool */
  VkCommandPool commandPool;
  new_CommandPool(&commandPool, device, graphicsIndex);

  /* get preferred format of screen*/
  VkSurfaceFormatKHR surfaceFormat;
  getPreferredSurfaceFormat(&surfaceFormat, physicalDevice, surface);

  /*Create swap chain */
  VkSwapchainKHR swapChain;
  uint32_t swapChainImageCount = 0;
  new_SwapChain(&swapChain, &swapChainImageCount, VK_NULL_HANDLE, surfaceFormat,
                physicalDevice, device, surface, swapChainExtent, graphicsIndex,
                presentIndex);

  VkImage *pSwapChainImages = NULL;
  VkImageView *pSwapChainImageViews = NULL;

  new_SwapChainImages(&pSwapChainImages, &swapChainImageCount, device,
                      swapChain);
  new_SwapChainImageViews(&pSwapChainImageViews, device, surfaceFormat.format,
                          swapChainImageCount, pSwapChainImages);

  /* Create depth buffer */
  VkDeviceMemory depthImageMemory;

  VkImage depthImage;
  new_DepthImage(&depthImage, &depthImageMemory, swapChainExtent,
                 physicalDevice, device);
  VkImageView depthImageView;
  new_DepthImageView(&depthImageView, device, depthImage);

  VkShaderModule fragShaderModule;
  {
    uint32_t *fragShaderFileContents;
    uint32_t fragShaderFileLength;
    readShaderFile("assets/shaders/shader.frag.spv", &fragShaderFileLength,
                   &fragShaderFileContents);
    new_ShaderModule(&fragShaderModule, device, fragShaderFileLength,
                     fragShaderFileContents);
    free(fragShaderFileContents);
  }

  VkShaderModule vertShaderModule;
  {
    uint32_t *vertShaderFileContents;
    uint32_t vertShaderFileLength;
    readShaderFile("assets/shaders/shader.vert.spv", &vertShaderFileLength,
                   &vertShaderFileContents);
    new_ShaderModule(&vertShaderModule, device, vertShaderFileLength,
                     vertShaderFileContents);
    free(vertShaderFileContents);
  }

  /* Create graphics pipeline */
  VkRenderPass renderPass;
  new_VertexDisplayRenderPass(&renderPass, device, surfaceFormat.format);

  VkPipelineLayout graphicsPipelineLayout;
  new_VertexDisplayPipelineLayout(&graphicsPipelineLayout, device);

  VkPipeline graphicsPipeline;
  new_VertexDisplayPipeline(&graphicsPipeline, device, vertShaderModule,
                            fragShaderModule, swapChainExtent, renderPass,
                            graphicsPipelineLayout);

  VkFramebuffer *pSwapChainFramebuffers;
  new_SwapChainFramebuffers(&pSwapChainFramebuffers, device, renderPass,
                            swapChainExtent, swapChainImageCount,
                            depthImageView, pSwapChainImageViews);

  VkCommandBuffer *pVertexDisplayCommandBuffers;
  new_VertexDisplayCommandBuffers(&pVertexDisplayCommandBuffers, device,
                                  renderPass, graphicsPipeline, commandPool,
                                  swapChainExtent, swapChainImageCount,
                                  pSwapChainFramebuffers);

  VkSemaphore *pImageAvailableSemaphores;
  VkSemaphore *pRenderFinishedSemaphores;
  VkFence *pInFlightFences;
  new_Semaphores(&pImageAvailableSemaphores, swapChainImageCount, device);
  new_Semaphores(&pRenderFinishedSemaphores, swapChainImageCount, device);
  new_Fences(&pInFlightFences, swapChainImageCount, device);

  uint32_t currentFrame = 0;
  /*wait till close*/
  while (!glfwWindowShouldClose(pWindow)) {
    glfwPollEvents();
    uint32_t result = drawFrame(
        &currentFrame, 2, device, swapChain, pVertexDisplayCommandBuffers,
        pInFlightFences, pImageAvailableSemaphores, pRenderFinishedSemaphores,
        graphicsQueue, presentQueue);

    if (result == ERR_OUTOFDATE) {
      vkDeviceWaitIdle(device);
      delete_Fences(&pInFlightFences, swapChainImageCount, device);
      delete_Semaphores(&pRenderFinishedSemaphores, swapChainImageCount,
                        device);
      delete_Semaphores(&pImageAvailableSemaphores, swapChainImageCount,
                        device);
      delete_VertexDisplayCommandBuffers(&pVertexDisplayCommandBuffers);
      delete_CommandPool(&commandPool, device);
      delete_SwapChainFramebuffers(&pSwapChainFramebuffers, swapChainImageCount,
                                   device);
      delete_Pipeline(&graphicsPipeline, device);
      delete_PipelineLayout(&graphicsPipelineLayout, device);
      delete_RenderPass(&renderPass, device);
      delete_SwapChainImageViews(&pSwapChainImageViews, swapChainImageCount,
                                 device);
      delete_SwapChainImages(&pSwapChainImages);
      delete_SwapChain(&swapChain, device);

      /* Set swapchain to new window size */
      getWindowExtent(&swapChainExtent, pWindow);

      /*Create swap chain */
      new_SwapChain(&swapChain, &swapChainImageCount, VK_NULL_HANDLE,
                    surfaceFormat, physicalDevice, device, surface,
                    swapChainExtent, graphicsIndex, presentIndex);
      new_SwapChainImages(&pSwapChainImages, &swapChainImageCount, device,
                          swapChain);
      new_SwapChainImageViews(&pSwapChainImageViews, device,
                              surfaceFormat.format, swapChainImageCount,
                              pSwapChainImages);

      /* Create graphics pipeline */
      new_VertexDisplayRenderPass(&renderPass, device, surfaceFormat.format);
      new_VertexDisplayPipelineLayout(&graphicsPipelineLayout, device);
      new_VertexDisplayPipeline(&graphicsPipeline, device, vertShaderModule,
                                fragShaderModule, swapChainExtent, renderPass,
                                graphicsPipelineLayout);
      new_SwapChainFramebuffers(&pSwapChainFramebuffers, device, renderPass,
                                swapChainExtent, swapChainImageCount,
                                depthImageView, pSwapChainImageViews);
      new_CommandPool(&commandPool, device, graphicsIndex);
      new_VertexDisplayCommandBuffers(&pVertexDisplayCommandBuffers, device,
                                      renderPass, graphicsPipeline, commandPool,
                                      swapChainExtent, swapChainImageCount,
                                      pSwapChainFramebuffers);
      new_Semaphores(&pImageAvailableSemaphores, swapChainImageCount, device);
      new_Semaphores(&pRenderFinishedSemaphores, swapChainImageCount, device);
      new_Fences(&pInFlightFences, swapChainImageCount, device);
    }
  }

  /*cleanup*/
  vkDeviceWaitIdle(device);
  delete_ShaderModule(&fragShaderModule, device);
  delete_ShaderModule(&vertShaderModule, device);
  delete_Fences(&pInFlightFences, swapChainImageCount, device);
  delete_Semaphores(&pRenderFinishedSemaphores, swapChainImageCount, device);
  delete_Semaphores(&pImageAvailableSemaphores, swapChainImageCount, device);
  delete_CommandBuffers(&pVertexDisplayCommandBuffers, commandPool, device);
  delete_CommandPool(&commandPool, device);
  delete_SwapChainFramebuffers(&pSwapChainFramebuffers, swapChainImageCount,
                               device);
  delete_Pipeline(&graphicsPipeline, device);
  delete_PipelineLayout(&graphicsPipelineLayout, device);
  delete_RenderPass(&renderPass, device);
  delete_SwapChainImageViews(&pSwapChainImageViews, swapChainImageCount,
                             device);
  delete_SwapChainImages(&pSwapChainImages);
  delete_SwapChain(&swapChain, device);
  delete_Device(&device);
  delete_Surface(&surface, instance);
  delete_DebugCallback(&callback, instance);
  delete_Instance(&instance);

  glfwTerminate();
  return (EXIT_SUCCESS);
}
