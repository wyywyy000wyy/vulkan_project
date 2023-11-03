#include "vkPhysicalSelector.h"
#include "vklibLogger.h"
#include <vector>

struct QueueFamilyIndices {
	uint32_t presentFamily = -1;
	bool isComplete() {
		return presentFamily != -1;
	}
};



QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		VK_LOG("Queue Family :", i);
		VK_LOG("  Supports Graphics: ", ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0));
		VK_LOG("  Supports Compute: ", ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0));
		VK_LOG("  Supports Transfer: ", ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0));
		VK_LOG("  Supports Sparse Binding: ", ((queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0));
		VK_LOG("  Supports Protected: ", ((queueFamilies[i].queueFlags & VK_QUEUE_PROTECTED_BIT) != 0));
	}

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.isComplete();
}


VkPhysicalDevice vkPhysicalSelector::selectPhysicalDevice(VkInstance instance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		VK_LOG_ERROR("failed to find GPUs with Vulkan support!");
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	VkPhysicalDevice physicalDevice = nullptr;
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == nullptr)
		throw std::runtime_error("failed to find a suitable GPU!");

	return physicalDevice;
}


uint32_t vkPhysicalSelector::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}
	return indices.presentFamily;
}