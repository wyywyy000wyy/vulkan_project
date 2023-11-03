#include "vkPhysicalSelector.h"
#include "vklibLogger.h"
#include <vector>

//QueueFamilyIndices vkPhysicalSelector::findQueueFamilies(VkPhysicalDevice device) {
//	QueueFamilyIndices indices;
//
//	uint32_t queueFamilyCount = 0;
//	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//
//	for (uint32_t i = 0; i < queueFamilyCount; i++) {
//		VK_LOG("Queue Family :", i);
//		VK_LOG("  Supports Graphics: ", ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0));
//		VK_LOG("  Supports Compute: ", ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0));
//		VK_LOG("  Supports Transfer: ", ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0));
//		VK_LOG("  Supports Sparse Binding: ", ((queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0));
//		VK_LOG("  Supports Protected: ", ((queueFamilies[i].queueFlags & VK_QUEUE_PROTECTED_BIT) != 0));
//	}
//
//	int i = 0;
//	for (const auto& queueFamily : queueFamilies) {
//		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//			indices.graphicsFamily = i;
//		}
//
//		VkBool32 presentSupport = false;
//		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
//
//		if (presentSupport) {
//			indices.presentFamily = i;
//		}
//
//		if (indices.isComplete()) {
//			break;
//		}
//
//		i++;
//	}
//
//	return indices;
//}





