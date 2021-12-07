///
/// Copyright 2019 Govind Pimpale
/// vulkan_methods.h
///
///  Created on: Aug 8, 2018
///      Author: gpi
///

#ifndef SRC_VULKAN_UTILS_H_
#define SRC_VULKAN_UTILS_H_

#include <stdbool.h>
#include <stdint.h>

#include <vulkan/vulkan.h>

#include <linmath.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "errors.h"

typedef struct {
  vec3 position;
  vec3 color;
} Vertex;

/// Creates a new VkInstance with the specified extensions and layers
/// --- PRECONDITIONS ---
/// * `ppEnabledExtensionNames` must be a pointer to at least
/// * `enabledExtensionCount` extensions `ppEnabledLayerNames` must be a pointer
/// to at least `enabledLayerCount` layers
/// * `appname` is a UTF8 null terminated string
/// --- POSTCONDITONS ---
/// * Returns the error status
/// * If enableGLFWRequiredExtensions, then all extensions needed by GLFW will
/// be enabled
/// * If enableDebugRequiredExtensions, then the extensions needed for
/// new_DebugCallback will be enabled
/// --- PANICS ---
/// Panics if memory allocation fails
/// Panics if Vulkan is not supported by GLFW
/// --- CLEANUP ---
/// To cleanup, call delete_Instance on the created instance
ErrVal new_Instance(                            //
    VkInstance *pInstance,                      //
    const uint32_t enabledLayerCount,           //
    const char *const *ppEnabledLayerNames,     //
    const uint32_t enabledExtensionCount,       //
    const char *const *ppEnabledExtensionNames, //
    const bool enableGLFWRequiredExtensions,    //
    const bool enableDebugRequiredExtensions,   //
    const char *appname                         //
);

/// Destroys the instance created in new_Instance
/// --- PRECONDITIONS ---
/// pInstance is a valid pointer to an instance created by new_Instance
/// --- POSTCONDITONS ---
/// the instance is no longer valid
/// pInstance has been set to VK_NULL_HANDLE
void delete_Instance(VkInstance *pInstance);

/// Creates a new window with GLFW
/// --- PRECONDITIONS ---
/// ppGLFWwindow is a valid pointer
/// --- POSTCONDITONS ---
/// returns error status
/// on success, *ppGlfwWindow is a valid GLFW window
ErrVal new_GlfwWindow(         //
    GLFWwindow **ppGlfwWindow, //
    const char *name,          //
    VkExtent2D dimensions      //
);

/// Gets the size of the framebuffer of the window
/// --- PRECONDITIONS ---
/// * `pExtent` is a valid pointer
/// * `pWindow` is a valid pointer to a GLFWwindow
/// --- POSTCONDITONS ---
/// * returns error status
/// * on success, pExtent is set to the size of the framebuffer of pWindow
ErrVal getExtentWindow(VkExtent2D *pExtent, GLFWwindow *pWindow);

/// Sets a debug callback to log error messages from validation layers
/// --- PRECONDITIONS ---
/// * `instance` must have been created with enableDebugRequiredExtensions true
/// * `pCallback` must be a valid pointer
/// --- POSTCONDITONS ---
/// * returns error status
/// * on success, `*pCallback` is set to a valid callback
/// * on success, will log all errors to stdout
/// --- CLEANUP ---
/// * call delete_DebugCallback on the created callback
ErrVal new_DebugCallback(                //
    VkDebugUtilsMessengerEXT *pCallback, //
    const VkInstance instance            //
);

/// Destroys the debug callback
/// --- PRECONDITIONS ---
/// `*pCallback` must have been created with `new_DebugCallback`
/// `instance` must be the instance with which `pCallback` was created
/// --- POSTCONDITONS ---
/// `*pCallback` is no longer a valid callback
/// `*pCallback` is set to VK_NULL_HANDLE
void delete_DebugCallback(               //
    VkDebugUtilsMessengerEXT *pCallback, //
    const VkInstance instance            //
);

/// Gets a the first physical device with both graphics and compute capabilities
/// --- PRECONDITIONS ---
/// `pDevice` must be a valid pointer
/// `instance` must be a valid instance
/// --- POSTCONDITONS ---
/// * returns error status
/// * on success, sets `*pDevice` to a valid physical device supporting graphics
/// and compute
ErrVal getPhysicalDevice(VkPhysicalDevice *pDevice, const VkInstance instance);

/// Creates a new logical device with the given physical device
/// --- PRECONDITIONS ---
/// * `pDevice` must be a valid pointer
/// * `physicalDevice` must be a valid physical device created from
/// * `getPhysicalDevice` `queueFamilyIndex` must be the index of the queue
/// family to use `ppEnabledExtensionNames` must be a pointer to at least
/// * `enabledExtensionCount` extensions
/// --- POSTCONDITIONS ---
/// returns error status
/// on success, `*pDevice` will be a new logical device
/// --- CLEANUP ---
/// call delete_Device
ErrVal new_Device(                             //
    VkDevice *pDevice,                         //
    const VkPhysicalDevice physicalDevice,     //
    const uint32_t queueFamilyIndex,           //
    const uint32_t enabledExtensionCount,      //
    const char *const *ppEnabledExtensionNames //
);

/// Deletes a logical device created from new_Device
/// --- PRECONDITIONS ---
/// * `pDevice` must be a valid pointer to a logical device created from
/// new_Device
/// --- POSTCONDITIONS ---
/// * `*pDevice` is no longer a valid logical device
/// * `*pDevice` is set to VK_NULL_HANDLE
void delete_Device(VkDevice *pDevice);

/// Gets the first queue family index with the stated capabilities
/// --- PRECONDITIONS ---
/// `pQueueFamilyIndex` must be a valid pointer
/// `device` must be created by getPhysicalDevice
/// --- POSTCONDITIONS ---
/// sets `*pQueueFamilyIndex` contains the index of the first queue family
/// supporting `bit`
ErrVal getQueueFamilyIndexByCapability( //
    uint32_t *pQueueFamilyIndex,        //
    const VkPhysicalDevice device,      //
    const VkQueueFlags bit              //
);

/// Gets the first queue family index which can support rendering to `surface`
/// --- PRECONDITIONS ---
/// * `pQueueFamilyIndex` must be a valid pointer
/// * `device` must be created by getPhysicalDevice
/// * `surface` must be a valid surface from the same instance as
/// `physicalDevice`
/// --- POSTCONDITIONS ---
/// * returns error status.
/// * on success, sets `*pQueueFamilyIndex` contains the index of the first
/// queue supporting presenting to `surface`
ErrVal getPresentQueueFamilyIndex(         //
    uint32_t *pQueueFamilyIndex,           //
    const VkPhysicalDevice physicalDevice, //
    const VkSurfaceKHR surface             //
);

/// Gets the queue associated with the queue family
/// --- PRECONDITIONS ---
/// * `pQueue` is a valid pointer
/// * `device` is a logical device created by `new_Device`
/// * `queueFamilyIndex` is a valid index for a queue family in the
/// corresponding physical device
/// --- POSTCONDITIONS ---
/// * returns error status
/// * on success, `pQueue` is set to a new 	queue in the given queue family
ErrVal getQueue(                    //
    VkQueue *pQueue,                //
    const VkDevice device,          //
    const uint32_t queueFamilyIndex //
);

/// Gets a surface format that can be rendered to
/// --- PRECONDITIONS ---
/// * `pSurfaceFormat` is a valid pointer
/// * `surface` has been allocated from the same instance as `physicalDevice`
/// --- POSTCONDITIONS ---
/// * returns error status
/// * `*pSurfaceFormat` contains a format that we can render to
ErrVal getPreferredSurfaceFormat(VkSurfaceFormatKHR *pSurfaceFormat,
                                 const VkPhysicalDevice physicalDevice,
                                 const VkSurfaceKHR surface);

/// Creates a new swapchain, possibly reusing the old one
/// --- PRECONDITIONS ---
/// * All vulkan objects come from the same instance
/// * `oldSwapchain` is a valid pointer
/// * `pSwapchainImageCount` is a valid pointer
/// * `oldSwapchain` is either VK_NULL_HANDLE or a swapchain created from
/// new_Swapchain
/// * `surfaceFormat` is from getPreferredSurfaceFormat called with
/// `physicalDevice` and `surface`
/// * `surface` has been allocated from `physicalDevice`
/// * `device` has been allocated from `physicalDevice`
/// * `extent` is the current extent of `surface`
/// * `graphicsIndex` is the queue family index for graphics operations
/// * `presentIndex` is the queue family index to submit present operations
/// --- POSTCONDITIONS ---
/// * returns error status
/// * on success, `*pSwapchain` is set to a new swapchain
/// * on success, `*pSwapchainImageCount` is set to the number of images in the
/// swapchain
/// --- CLEANUP ---
/// * call `delete_Swapchain` to free resources associated with this swapchain
ErrVal new_Swapchain(                       //
    VkSwapchainKHR *pSwapchain,             //
    uint32_t *pSwapchainImageCount,         //
    const VkSwapchainKHR oldSwapchain,      //
    const VkSurfaceFormatKHR surfaceFormat, //
    const VkPhysicalDevice physicalDevice,  //
    const VkDevice device,                  //
    const VkSurfaceKHR surface,             //
    const VkExtent2D extent,                //
    const uint32_t graphicsIndex,           //
    const uint32_t presentIndex             //
);

/// Deletes a swapchain created from new_Swapchain
/// --- PRECONDITIONS ---
/// * `pSwapchain` must be a valid pointer to a swapchain created from
/// new_Swapchain
/// * `device` must be the logical device from which `*pSwapchain` was allocated
/// --- POSTCONDITIONS ---
/// * Resources associated with `*pSwapchain` have been released
/// * `*pSwapchain` is no longer a valid swapchain
/// * `*pSwapchain` is set to VK_NULL_HANDLE
void delete_Swapchain(VkSwapchainKHR *pSwapchain, const VkDevice device);

/// gets swapchain images from the swapchain
/// --- PRECONDITIONS --
/// * swapchain
ErrVal getSwapchainImages(         //
    VkImage *pSwapchainImages,     //
    const uint32_t imageCount,     //
    const VkDevice device,         //
    const VkSwapchainKHR swapchain //
);

ErrVal new_Image(                           //
    VkImage *pImage,                        //
    VkDeviceMemory *pImageMemory,           //
    const VkExtent2D dimensions,            //
    const VkFormat format,                  //
    const VkImageTiling tiling,             //
    const VkImageUsageFlags usage,          //
    const VkMemoryPropertyFlags properties, //
    const VkPhysicalDevice physicalDevice,  //
    const VkDevice device                   //
);

/// Deletes a image created from new_Image
/// --- PRECONDITIONS ---
/// * `pImage` must be a valid pointer to a image created from new_Image
/// * `device` must be the logical device from which `*pImage` was allocated
/// --- POSTCONDITIONS ---
/// * Resources associated with `*pImage` have been released
/// * `*pImage` is no longer a valid image
/// * `*pImage` is set to VK_NULL_HANDLE
void delete_Image(VkImage *pImage, const VkDevice device);

ErrVal new_ImageView(         //
    VkImageView *pImageView,  //
    const VkDevice device,    //
    const VkImage image,      //
    const VkFormat format,    //
    const uint32_t aspectMask //
);

/// Deletes a imageView created from new_ImageView
/// --- PRECONDITIONS ---
/// * `pImageView` must be a valid pointer to a imageView created from
/// new_ImageView
/// * `device` must be the logical device from which `*pImageView` was allocated
/// --- POSTCONDITIONS ---
/// * Resources associated with `*pImageView` have been released
/// * `*pImageView` is no longer a valid imageView
/// * `*pImageView` is set to VK_NULL_HANDLE
void delete_ImageView(VkImageView *pImageView, VkDevice device);

ErrVal new_SwapchainImageViews(      //
    VkImageView *pImageViews,        //
    const VkImage *pSwapchainImages, //
    const uint32_t imageCount,       //
    const VkDevice device,           //
    const VkFormat format            //
);

void delete_SwapchainImageViews( //
    VkImageView *pImageViews,    //
    const uint32_t imageCount,   //
    const VkDevice device        //
);

ErrVal new_ShaderModule(VkShaderModule *pShaderModule, const VkDevice device,
                        const uint32_t codeSize, const uint32_t *pCode);

/// Deletes a shaderModule created from new_ShaderModule
/// --- PRECONDITIONS ---
/// * `pShaderModule` must be a valid pointer to a shaderModule created from
/// new_ShaderModule
/// * `device` must be the logical device from which `*pShaderModule` was
/// allocated
/// --- POSTCONDITIONS ---
/// * Resources associated with `*pShaderModule` have been released
/// * `*pShaderModule` is no longer a valid shaderModule
/// * `*pShaderModule` is set to VK_NULL_HANDLE
void delete_ShaderModule(VkShaderModule *pShaderModule, const VkDevice device);

ErrVal new_VertexDisplayRenderPass(VkRenderPass *pRenderPass,
                                   const VkDevice device,
                                   const VkFormat swapchainImageFormat);

void delete_RenderPass(VkRenderPass *pRenderPass, const VkDevice device);

ErrVal new_VertexDisplayPipelineLayout(VkPipelineLayout *pPipelineLayout,
                                       const VkDevice device);

void delete_PipelineLayout(VkPipelineLayout *pPipelineLayout,
                           const VkDevice device);

ErrVal new_VertexDisplayPipeline(VkPipeline *pVertexDisplayPipeline,
                                 const VkDevice device,
                                 const VkShaderModule vertShaderModule,
                                 const VkShaderModule fragShaderModule,
                                 const VkExtent2D extent,
                                 const VkRenderPass renderPass,
                                 const VkPipelineLayout pipelineLayout);

void delete_Pipeline(VkPipeline *pPipeline, const VkDevice device);

ErrVal new_Framebuffer(VkFramebuffer *pFramebuffer, const VkDevice device,
                       const VkRenderPass renderPass,
                       const VkImageView imageView,
                       const VkImageView depthImageView,
                       const VkExtent2D swapchainExtent);

void delete_Framebuffer(VkFramebuffer *pFramebuffer, VkDevice device);

ErrVal new_SwapchainFramebuffers(VkFramebuffer **ppFramebuffers,
                                 const VkDevice device,
                                 const VkRenderPass renderPass,
                                 const VkExtent2D swapchainExtent,
                                 const uint32_t imageCount,
                                 const VkImageView depthImageView,
                                 const VkImageView *pSwapchainImageViews);

void delete_SwapchainFramebuffers(VkFramebuffer **ppFramebuffers,
                                  const uint32_t imageCount,
                                  const VkDevice device);

ErrVal new_CommandPool(VkCommandPool *pCommandPool, const VkDevice device,
                       const uint32_t queueFamilyIndex);

void delete_CommandPool(VkCommandPool *pCommandPool, const VkDevice device);

ErrVal new_VertexDisplayCommandBuffers(                 //
    VkCommandBuffer *pCommandBuffers,                   //
    const uint32_t swapchainFramebufferCount,           //
    const VkFramebuffer *pSwapchainFramebuffers,        //
    const VkBuffer vertexBuffer,                        //
    const uint32_t vertexCount,                         //
    const VkDevice device,                              //
    const VkRenderPass renderPass,                      //
    const VkPipelineLayout vertexDisplayPipelineLayout, //
    const VkPipeline vertexDisplayPipeline,             //
    const VkCommandPool commandPool,                    //
    const VkExtent2D swapchainExtent,                   //
    const mat4x4 cameraTransform                        //
);

void delete_CommandBuffers(VkCommandBuffer **ppCommandBuffers,
                           const uint32_t commandBufferCount,
                           const VkCommandPool commandPool,
                           const VkDevice device);

ErrVal new_Semaphore(VkSemaphore *pSemaphore, const VkDevice device);

void delete_Semaphore(VkSemaphore *pSemaphore, const VkDevice device);

ErrVal new_Semaphores(VkSemaphore *pSemaphores, const uint32_t semaphoreCount,
                      const VkDevice device);

void delete_Semaphores(VkSemaphore *pSemaphores, const uint32_t semaphoreCount,
                       const VkDevice device);

ErrVal new_Fence(VkFence *pFence, const VkDevice device);

void delete_Fence(VkFence *pFence, const VkDevice device);

ErrVal new_Fences(VkFence *pFences, const uint32_t fenceCount,
                  const VkDevice device);

void delete_Fences(VkFence *pFences, const uint32_t fenceCount,
                   const VkDevice device);

ErrVal drawFrame(uint32_t *pCurrentFrame, const uint32_t maxFramesInFlight,
                 const VkDevice device, const VkSwapchainKHR swapchain,
                 const VkCommandBuffer *pCommandBuffers,
                 const VkFence *pInFlightFences,
                 const VkSemaphore *pImageAvailableSemaphores,
                 const VkSemaphore *pRenderFinishedSemaphores,
                 const VkQueue graphicsQueue, const VkQueue presentQueue);

ErrVal new_SurfaceFromGLFW(VkSurfaceKHR *pSurface, GLFWwindow *pWindow,
                           const VkInstance instance);

void delete_Surface(VkSurfaceKHR *pSurface, const VkInstance instance);

ErrVal new_VertexBuffer(VkBuffer *pBuffer, VkDeviceMemory *pBufferMemory,
                        const Vertex *pVertices, const uint32_t vertexCount,
                        const VkDevice device,
                        const VkPhysicalDevice physicalDevice,
                        const VkCommandPool commandPool, const VkQueue queue);

ErrVal new_Buffer_DeviceMemory(VkBuffer *pBuffer, VkDeviceMemory *pBufferMemory,
                               const VkDeviceSize size,
                               const VkPhysicalDevice physicalDevice,
                               const VkDevice device,
                               const VkBufferUsageFlags usage,
                               const VkMemoryPropertyFlags properties);

ErrVal copyBuffer(VkBuffer destinationBuffer, const VkBuffer sourceBuffer,
                  const VkDeviceSize size, const VkCommandPool commandPool,
                  const VkQueue queue, const VkDevice device);

void delete_Buffer(VkBuffer *pBuffer, const VkDevice device);

void delete_DeviceMemory(VkDeviceMemory *pDeviceMemory, const VkDevice device);

ErrVal new_begin_OneTimeSubmitCommandBuffer(VkCommandBuffer *pCommandBuffer,
                                            const VkDevice device,
                                            const VkCommandPool commandPool);

ErrVal delete_end_OneTimeSubmitCommandBuffer(VkCommandBuffer *pCommandBuffer,
                                             const VkDevice device,
                                             const VkQueue queue,
                                             const VkCommandPool commandPool);

ErrVal copyToDeviceMemory(VkDeviceMemory *pDeviceMemory,
                          const VkDeviceSize deviceSize, const void *source,
                          const VkDevice device);

void getDepthFormat(VkFormat *pFormat);

ErrVal new_DepthImageView(VkImageView *pImageView, const VkDevice device,
                          const VkImage depthImage);

ErrVal new_DepthImage(VkImage *pImage, VkDeviceMemory *pImageMemory,
                      const VkExtent2D swapchainExtent,
                      const VkPhysicalDevice physicalDevice,
                      const VkDevice device);

ErrVal getMemoryTypeIndex(uint32_t *memoryTypeIndex,
                          const uint32_t memoryTypeBits,
                          const VkMemoryPropertyFlags memoryPropertyFlags,
                          const VkPhysicalDevice physicalDevice);

ErrVal new_ComputePipeline(VkPipeline *pPipeline,
                           const VkPipelineLayout pipelineLayout,
                           const VkShaderModule shaderModule,
                           const VkDevice device);

ErrVal new_ComputeStorageDescriptorSetLayout(
    VkDescriptorSetLayout *pDescriptorSetLayout, const VkDevice device);

void delete_DescriptorSetLayout(VkDescriptorSetLayout *pDescriptorSetLayout,
                                const VkDevice device);

ErrVal new_DescriptorPool(VkDescriptorPool *pDescriptorPool,
                          const VkDescriptorType descriptorType,
                          const uint32_t maxAllocFrom, const VkDevice device);

void delete_DescriptorPool(VkDescriptorPool *pDescriptorPool,
                           const VkDevice device);

ErrVal new_ComputeBufferDescriptorSet(
    VkDescriptorSet *pDescriptorSet, const VkBuffer computeBufferDescriptorSet,
    const VkDeviceSize computeBufferSize,
    const VkDescriptorSetLayout descriptorSetLayout,
    const VkDescriptorPool descriptorPool, const VkDevice device);

void delete_DescriptorSets(VkDescriptorSet **ppDescriptorSets);

#endif /* SRC_VULKAN_UTILS_H_ */
