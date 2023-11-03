#pragma once
#if __ANDROID__
#include <vulkan_wrapper.h>
#else
#include <vulkan/vulkan.h>
#endif

class vkPhysicalSelector
{
public:
	//static VkPhysicalDevice selectPhysicalDevice(VkInstance instance);
    //static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	//static uint32_t findQueueFamilies(VkPhysicalDevice device);
};