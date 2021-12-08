#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define APPNAME "Vulkan Triangle"

#include "camera.h"
#include "utils.h"
#include "vulkan_utils.h"

#include "errors.h"

#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500
#define MAX_FRAMES_IN_FLIGHT 2

static uint32_t vertexCount = 6;
static Vertex vertexData[] = {
    (Vertex){.position = {1.0, 0.0, 0.0}, .color = {1.0, 0.0, 0.0}},
    (Vertex){.position = {0.0, 1.0, 0.0}, .color = {0.0, 1.0, 0.0}},
    (Vertex){.position = {0.0, 0.0, 1.0}, .color = {0.0, 0.0, 1.0}},
    (Vertex){.position = {1.0, 0.0, 1.0}, .color = {1.0, 0.0, 0.0}},
    (Vertex){.position = {0.0, 1.0, 0.0}, .color = {0.0, 1.0, 0.0}},
    (Vertex){.position = {1.0, 0.0, 1.0}, .color = {0.0, 0.0, 1.0}},
};

int main(void) {
  glfwInit();

  const uint32_t validationLayerCount = 1;
  const char *ppValidationLayerNames[1] = {"VK_LAYER_KHRONOS_validation"};

  /* Create instance */
  VkInstance instance;
  new_Instance(&instance, validationLayerCount, ppValidationLayerNames, 0, NULL,
               true, true, APPNAME);

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
  getExtentWindow(&swapchainExtent, pWindow);

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

  VkFramebuffer *pSwapchainFramebuffers =
      malloc(swapchainImageCount * sizeof(VkFramebuffer));
  new_SwapchainFramebuffers(pSwapchainFramebuffers, device, renderPass,
                            swapchainExtent, swapchainImageCount,
                            depthImageView, pSwapchainImageViews);

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  new_VertexBuffer(&vertexBuffer, &vertexBufferMemory, vertexData, vertexCount,
                   device, physicalDevice, commandPool, graphicsQueue);

  VkCommandBuffer pVertexDisplayCommandBuffers[MAX_FRAMES_IN_FLIGHT];
  new_CommandBuffers(pVertexDisplayCommandBuffers, 2, commandPool, device);

  // Create image synchronization primitives
  VkSemaphore pImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
  new_Semaphores(pImageAvailableSemaphores, MAX_FRAMES_IN_FLIGHT, device);
  VkSemaphore pRenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
  new_Semaphores(pRenderFinishedSemaphores, MAX_FRAMES_IN_FLIGHT, device);
  VkFence pInFlightFences[MAX_FRAMES_IN_FLIGHT];
  new_Fences(pInFlightFences, MAX_FRAMES_IN_FLIGHT, device,
             true); // fences start off signaled

  // create camera
  vec3 loc = {0.0f, 0.0f, 0.0f};
  Camera camera = new_Camera(loc, swapchainExtent);

  // this number counts which frame we're on
  // up to MAX_FRAMES_IN_FLIGHT, at whcich points it resets to 0
  uint32_t currentFrame = 0;

  /*wait till close*/
  while (!glfwWindowShouldClose(pWindow)) {
    glfwPollEvents();

    // wait for last frame to finish
    waitAndResetFence(pInFlightFences[currentFrame], device);

    // the imageIndex is the index of the swapchain framebuffer that is
    // available next
    uint32_t imageIndex;
    // this function will return immediately,
    //  so we use the semaphore to tell us when the image is actually available,
    //  (ready for rendering to)
    ErrVal result =
        getNextSwapchainImage(&imageIndex, swapchain, device,
                              pImageAvailableSemaphores[currentFrame]);

    // if the window is resized
    if (result == ERR_OUTOFDATE) {
      vkDeviceWaitIdle(device);

      delete_SwapchainFramebuffers(pSwapchainFramebuffers, swapchainImageCount,
                                   device);
      free(pSwapchainFramebuffers);
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
      getExtentWindow(&swapchainExtent, pWindow);
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
      pSwapchainFramebuffers =
          malloc(swapchainImageCount * sizeof(VkFramebuffer));
      new_SwapchainFramebuffers(pSwapchainFramebuffers, device, renderPass,
                                swapchainExtent, swapchainImageCount,
                                depthImageView, pSwapchainImageViews);

      // finally we can retry getting the swapchain
      getNextSwapchainImage(&imageIndex, swapchain, device,
                            pImageAvailableSemaphores[currentFrame]);
    }

    // update camera
    updateCamera(&camera, pWindow);
    mat4x4 mvp;
    getMvpCamera(mvp, &camera);

    // record buffer
    recordVertexDisplayCommandBuffer(                //
        pVertexDisplayCommandBuffers[currentFrame],  //
        pSwapchainFramebuffers[imageIndex],          //
        vertexBuffer,                                //
        vertexCount,                                 //
        renderPass,                                  //
        graphicsPipelineLayout,                      //
        graphicsPipeline,                            //
        swapchainExtent,                             //
        mvp,                                         //
        (VkClearColorValue){.float32 = {0, 0, 0, 0}} //
    );

    drawFrame(                                      //
        pVertexDisplayCommandBuffers[currentFrame], //
        swapchain,                                  //
        imageIndex,                                 //
        pImageAvailableSemaphores[currentFrame],    //
        pRenderFinishedSemaphores[currentFrame],    //
        pInFlightFences[currentFrame],              //
        graphicsQueue,                              //
        presentQueue                                //
    );

    // increment frame
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  /*cleanup*/
  vkDeviceWaitIdle(device);
  delete_ShaderModule(&fragShaderModule, device);
  delete_ShaderModule(&vertShaderModule, device);

  delete_Fences(pInFlightFences, MAX_FRAMES_IN_FLIGHT, device);
  delete_Semaphores(pRenderFinishedSemaphores, MAX_FRAMES_IN_FLIGHT, device);
  delete_Semaphores(pImageAvailableSemaphores, MAX_FRAMES_IN_FLIGHT, device);

  delete_CommandBuffers(pVertexDisplayCommandBuffers, MAX_FRAMES_IN_FLIGHT,
                        commandPool, device);
  delete_CommandPool(&commandPool, device);

  delete_SwapchainFramebuffers(pSwapchainFramebuffers, swapchainImageCount,
                               device);
  free(pSwapchainFramebuffers);
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
