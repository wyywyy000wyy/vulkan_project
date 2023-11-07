#pragma once
#if __ANDROID__
#include <vulkan_wrapper.h>
#else
#include <vulkan/vulkan.h>
#endif
#include <vector>
#include "vklibLogger.h"

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

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

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
	virtual void createSurface() = 0;
	void createSwapChain();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
};