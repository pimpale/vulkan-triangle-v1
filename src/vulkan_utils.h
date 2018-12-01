struct VulkanWindow {
	VkInstance instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	GLFWwindow window;

	VkSurfaceKHR surface;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkExtent2D swapChainExtent;

};
