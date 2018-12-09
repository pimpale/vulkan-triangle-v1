struct VulkanWindow {
	GLFWwindow window;

	VkInstance instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;

	VkExtent2D swapChainExtent;

	uint32_t swapChainImageCount;
	/* One for each frame in the swap chain*/
	VkImage* pSwapChainImages;
	VkImageView* pSwapChainImageViews;
	VkFramebuffer* pSwapChainFramebuffers;
	VkCommandBuffer* pGraphicsCommandBuffers;
	VkSemaphore* pImageAvailableSemaphores;
	VkSemaphore* pRenderFinishedSemaphores;
	VkFence* pInFlightFences;
};

uint32_t new_VulkanWindow(struct VulkanWindow* pVulkanWindow,
		const VkInstance instance, const VkPhysicalDevice physicalDevice,
		const VkDevice device);
