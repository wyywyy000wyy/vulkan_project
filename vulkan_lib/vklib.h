#pragma once
#include "vklib_require.h"
#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	uint32_t graphicsFamily = -1;
	uint32_t presentFamily = -1;

	bool isComplete() {
		return graphicsFamily != -1 && presentFamily != -1;
	}
};


class vklib
{
public:
	void initVulkan();
	void initVulkan(std::vector<const char*>& platformExtensions, std::vector<const char*>& platformDeviceExtensions);
	void cleanup();
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	bool enableValidationLayers = false;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;


	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
	bool framebufferResized = false;
	void windSizeChanged();
	void draw();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;


	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
protected:
	void createInstance(std::vector<const char*>& platformExtensions, std::vector<const char*>& platformDeviceExtensions);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	bool checkValidationLayerSupport();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	VkPhysicalDevice selectPhysicalDevice(VkInstance instance);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void createLogicalDevice();
	std::vector<const char*> getRequiredExtensions();
	virtual void createSurface();
	void createSwapChain();
	void createSwapChainList();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();

	/// <summary>
	/// UniformBuffer
	/// </summary>
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSets();

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();

	void cleanupSwapChain();
	void recreateSwapChain();
};