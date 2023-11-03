#pragma once
#if !WIN32 && !MACOSX
#include <vulkan_wrapper.h>
#else
#include <vulkan/vulkan.h>
#endif

class vkPhysicalSelector
{
public:
	static VkPhysicalDevice selectPhysicalDevice(VkInstance instance);

	static uint32_t findQueueFamilies(VkPhysicalDevice device);
};