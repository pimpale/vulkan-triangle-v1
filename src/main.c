#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "camera.h"
#include "constants.h"
#include "errors.h"
#include "utils.h"
#include "vulkan_utils.h"

static uint32_t vertex_count = 3;
static Vertex vertex_data[] = {
    (Vertex){.position = {1.0, 0.0, 0.0}, .color = {1.0, 0.0, 0.0}},
    (Vertex){.position = {0.0, 1.0, 0.0}, .color = {0.0, 1.0, 0.0}},
    (Vertex){.position = {0.0, 0.0, 1.0}, .color = {0.0, 0.0, 1.0}},
};

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
    uint32_t ret1 = getQueueFamilyIndexByCapability(
        &graphicsIndex, physicalDevice, VK_QUEUE_GRAPHICS_BIT);
    uint32_t ret2 = getQueueFamilyIndexByCapability(
        &computeIndex, physicalDevice, VK_QUEUE_COMPUTE_BIT);
    uint32_t ret3 =
        getPresentQueueFamilyIndex(&presentIndex, physicalDevice, surface);
    /* Panic if indices are unavailable */
    if (ret1 != VK_SUCCESS || ret2 != VK_SUCCESS || ret3 != VK_SUCCESS) {
      LOG_ERROR(ERR_LEVEL_FATAL, "unable to acquire indices\n");
      PANIC();
    }
  }

  /* Set extent (for now just window width and height) */
  VkExtent2D swapchainExtent;
  getWindowExtent(&swapchainExtent, pWindow);

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

  /* Create swap chain */
  VkSwapchainKHR swapchain;
  uint32_t swapchainImageCount;
  new_Swapchain(&swapchain, &swapchainImageCount, VK_NULL_HANDLE, surfaceFormat,
                physicalDevice, device, surface, swapchainExtent, graphicsIndex,
                presentIndex);

  // there are swapchainImageCount swapchainImages
  VkImage *pSwapchainImages = malloc(swapchainImageCount * sizeof(VkImage));
  getSwapchainImages(pSwapchainImages, swapchainImageCount, device, swapchain);

  // there are swapchainImageCount swapchainImageViews
  VkImageView *pSwapchainImageViews =
      malloc(swapchainImageCount * sizeof(VkImageView));
  new_SwapchainImageViews(pSwapchainImageViews, pSwapchainImages,
                          swapchainImageCount, device, surfaceFormat.format);

  /* Create depth buffer */
  VkDeviceMemory depthImageMemory;
  VkImage depthImage;
  new_DepthImage(&depthImage, &depthImageMemory, swapchainExtent,
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
                            fragShaderModule, swapchainExtent, renderPass,
                            graphicsPipelineLayout);

  VkFramebuffer *pSwapchainFramebuffers;
  new_SwapchainFramebuffers(&pSwapchainFramebuffers, device, renderPass,
                            swapchainExtent, swapchainImageCount,
                            depthImageView, pSwapchainImageViews);

  VkSemaphore *pImageAvailableSemaphores =
      malloc(swapchainImageCount * sizeof(VkSemaphore));
  VkSemaphore *pRenderFinishedSemaphores =
      malloc(swapchainImageCount * sizeof(VkSemaphore));
  VkFence *pInFlightFences = malloc(swapchainImageCount * sizeof(VkFence));
  new_Semaphores(pImageAvailableSemaphores, swapchainImageCount, device);
  new_Semaphores(pRenderFinishedSemaphores, swapchainImageCount, device);
  new_Fences(pInFlightFences, swapchainImageCount, device);

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  new_VertexBuffer(&vertexBuffer, &vertexBufferMemory, vertex_data,
                   vertex_count, device, physicalDevice, commandPool,
                   graphicsQueue);

  // create camera
  vec3 loc = {0.0f, 0.0f, 0.0f};
  Camera camera = new_Camera(loc, swapchainExtent);

  uint32_t currentFrame = 0;

  /*wait till close*/
  while (!glfwWindowShouldClose(pWindow)) {
    // poll events and update camera
    glfwPollEvents();
    updateCamera(&camera, pWindow);

    mat4x4 mvp;
    getMvpCamera(mvp, &camera);

    VkCommandBuffer *pVertexDisplayCommandBuffers =
        malloc(swapchainImageCount * sizeof(VkCommandBuffer));

    new_VertexDisplayCommandBuffers(
        pVertexDisplayCommandBuffers, swapchainImageCount,
        pSwapchainFramebuffers, vertexBuffer, vertex_count, device, renderPass,
        graphicsPipelineLayout, graphicsPipeline, commandPool, swapchainExtent,
        mvp);

    uint32_t result = drawFrame(
        &currentFrame, 2, device, swapchain, pVertexDisplayCommandBuffers,
        pInFlightFences, pImageAvailableSemaphores, pRenderFinishedSemaphores,
        graphicsQueue, presentQueue);

    // delete_CommandBuffers(&pVertexDisplayCommandBuffers,swapchainImageCount,
    // commandPool, device);
    free(pVertexDisplayCommandBuffers);

    if (result == ERR_OUTOFDATE) {
      vkDeviceWaitIdle(device);
      delete_Fences(pInFlightFences, swapchainImageCount, device);
      delete_Semaphores(pRenderFinishedSemaphores, swapchainImageCount, device);
      delete_Semaphores(pImageAvailableSemaphores, swapchainImageCount, device);

      free(pInFlightFences);
      free(pRenderFinishedSemaphores);
      free(pImageAvailableSemaphores);

      delete_CommandPool(&commandPool, device);
      delete_SwapchainFramebuffers(&pSwapchainFramebuffers, swapchainImageCount,
                                   device);
      delete_Pipeline(&graphicsPipeline, device);
      delete_PipelineLayout(&graphicsPipelineLayout, device);
      delete_RenderPass(&renderPass, device);
      delete_SwapchainImageViews(pSwapchainImageViews, swapchainImageCount,
                                 device);
      free(pSwapchainImageViews);
      free(pSwapchainImages);
      delete_Swapchain(&swapchain, device);

      // delete depth buffer
      delete_ImageView(&depthImageView, device);
      delete_Image(&depthImage, device);
      delete_DeviceMemory(&depthImageMemory, device);

      // get new window size
      getWindowExtent(&swapchainExtent, pWindow);
      resizeCamera(&camera, swapchainExtent);

      /* recreate swap chain */
      new_Swapchain(&swapchain, &swapchainImageCount, swapchain, surfaceFormat,
                    physicalDevice, device, surface, swapchainExtent,
                    graphicsIndex, presentIndex);

      pSwapchainImages = malloc(swapchainImageCount * sizeof(VkImage));
      getSwapchainImages(pSwapchainImages, swapchainImageCount, device,
                         swapchain);

      pSwapchainImageViews = malloc(swapchainImageCount * sizeof(VkImageView));
      new_SwapchainImageViews(pSwapchainImageViews, pSwapchainImages,
                              swapchainImageCount, device,
                              surfaceFormat.format);

      // Create depth image
      new_DepthImage(&depthImage, &depthImageMemory, swapchainExtent,
                     physicalDevice, device);
      new_DepthImageView(&depthImageView, device, depthImage);

      /* Create graphics pipeline */
      new_VertexDisplayRenderPass(&renderPass, device, surfaceFormat.format);
      new_VertexDisplayPipelineLayout(&graphicsPipelineLayout, device);
      new_VertexDisplayPipeline(&graphicsPipeline, device, vertShaderModule,
                                fragShaderModule, swapchainExtent, renderPass,
                                graphicsPipelineLayout);
      new_SwapchainFramebuffers(&pSwapchainFramebuffers, device, renderPass,
                                swapchainExtent, swapchainImageCount,
                                depthImageView, pSwapchainImageViews);
      new_CommandPool(&commandPool, device, graphicsIndex);

      pImageAvailableSemaphores =
          malloc(swapchainImageCount * sizeof(VkSemaphore));
      pRenderFinishedSemaphores =
          malloc(swapchainImageCount * sizeof(VkSemaphore));
      pInFlightFences = malloc(swapchainImageCount * sizeof(VkFence));
      new_Semaphores(pImageAvailableSemaphores, swapchainImageCount, device);
      new_Semaphores(pRenderFinishedSemaphores, swapchainImageCount, device);
      new_Fences(pInFlightFences, swapchainImageCount, device);
    }
  }

  /*cleanup*/
  vkDeviceWaitIdle(device);
  delete_ShaderModule(&fragShaderModule, device);
  delete_ShaderModule(&vertShaderModule, device);
  delete_Fences(pInFlightFences, swapchainImageCount, device);
  delete_Semaphores(pRenderFinishedSemaphores, swapchainImageCount, device);
  delete_Semaphores(pImageAvailableSemaphores, swapchainImageCount, device);
  free(pInFlightFences);
  free(pRenderFinishedSemaphores);
  free(pImageAvailableSemaphores);
  delete_CommandPool(&commandPool, device);
  delete_SwapchainFramebuffers(&pSwapchainFramebuffers, swapchainImageCount,
                               device);
  delete_Pipeline(&graphicsPipeline, device);
  delete_PipelineLayout(&graphicsPipelineLayout, device);
  delete_Buffer(&vertexBuffer, device);
  delete_DeviceMemory(&vertexBufferMemory, device);
  delete_RenderPass(&renderPass, device);
  delete_SwapchainImageViews(pSwapchainImageViews, swapchainImageCount, device);
  free(pSwapchainImageViews);
  free(pSwapchainImages);
  delete_Swapchain(&swapchain, device);
  delete_ImageView(&depthImageView, device);
  delete_Image(&depthImage, device);
  delete_DeviceMemory(&depthImageMemory, device);
  delete_Device(&device);
  delete_Surface(&surface, instance);
  delete_DebugCallback(&callback, instance);
  delete_Instance(&instance);

  glfwTerminate();
  return (EXIT_SUCCESS);
}
