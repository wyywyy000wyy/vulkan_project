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

protected:
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
	void createInstance(std::vector<const char*>& platformExtensions, std::vector<const char*>& platformDeviceExtensions);
};