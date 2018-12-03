struct VulkanWindow {
	GLFWwindow window;

	VkInstance instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;


	VkExtent2D swapChainExtent;

	uint32_t swapChainImageCount;
	VkImage* pSwapChainImages;
	VkImageView pSwapChainImageViews;
	VkFramebuffer pFramebuffers;
};

uint32_t new_VulkanWindow(struct VulkanWindow* pVulkanWindow,
		const VkInstance instace, const VkPhysicalDevice physicalDevice,
		const VkDevice device);
