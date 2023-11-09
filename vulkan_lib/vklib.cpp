#include "vklib.h"
#include "vkfile.h"
#include "vkshader.h"
#include <iostream>
#include <array>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vklibMesh.h"
#include "vkPhysicalSelector.h"
#include <chrono>
#if SUPPORT_GLFW3_
#include <GLFW/glfw3.h>
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void printArgs(std::stringstream& ss)
{

}
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

extern vklibLogger* logger;

void vklib::initVulkan()
{
	std::vector<const char*> platformExtensions;
	std::vector<const char*> platformDeviceExtensions;
	initVulkan(platformExtensions, platformDeviceExtensions);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	VK_LOG_ERROR("validation layer: ", pCallbackData->pMessage);
	return VK_FALSE;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void vklib::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	//VK_CALL(vkCreateDebugUtilsMessengerEXT, instance, &createInfo, nullptr, &debugMessenger);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		VK_LOG_ERROR("failed to set up debug messenger!");
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void vklib::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = VkDebugUtilsMessengerCreateInfoEXT();
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

void vklib::initVulkan(std::vector<const char*>& platformExtensions, std::vector<const char*>& platformDeviceExtensions)
{
#if WIN32
	enableValidationLayers = true;
#endif
    createInstance(platformExtensions, platformDeviceExtensions);
	setupDebugMessenger();
	createSurface();
	VK_LOG("createSurfacee success!")
	pickPhysicalDevice();
	createLogicalDevice();
	createCommandPool();
	//createSwapChain();
	//createImageViews();
	//createRenderPass();
	//createDescriptorSetLayout();
	//createGraphicsPipeline();
	//createFramebuffers();
	//createUniformBuffers();
	//createCommandBuffers();
	createSwapChainList();
	createSyncObjects();
}

bool vklib::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> vklib::getRequiredExtensions() {
	std::vector<const char*> extensions;

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void vklib::createInstance(std::vector<const char*>& platformExtensions, std::vector<const char*>& platformDeviceExtensions)
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

    VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "vklib";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "vklib";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;
	
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	extensions.insert(extensions.end(), platformExtensions.begin(), platformExtensions.end());
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	VK_CALL(vkCreateInstance, &createInfo, nullptr, &instance);
}


void vklib::pickPhysicalDevice()
{
	physicalDevice = selectPhysicalDevice(instance);
}

bool vklib::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.isComplete();
}

VkPhysicalDevice vklib::selectPhysicalDevice(VkInstance instance)
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
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE)
	{
		VK_LOG_ERROR("failed to find a suitable GPU!");
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	return physicalDevice;
}


QueueFamilyIndices vklib::findQueueFamilies(VkPhysicalDevice device) {
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
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void vklib::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		VK_LOG_ERROR("failed to create logical device!");
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);

	//vkGetDeviceQueue(device, queueCreateInfo.queueFamilyIndex, 0, &graphicsQueue);
	VK_LOG("createLogicalDevice finish")
}



SwapChainSupportDetails vklib::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}



void vklib::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; 
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;


	VK_CALL(vkCreateSwapchainKHR, device, &createInfo, nullptr, &swapChain);

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}


VkSurfaceFormatKHR vklib::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	VkSurfaceFormatKHR ret = availableFormats[0];
	for(const auto& availableFormat : availableFormats)
	{
		VK_LOG("********availableFormat.format: ", availableFormat.format, " colorSpace: ", VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			ret = availableFormat;
		}
	}
	return ret;
}

VkPresentModeKHR vklib::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	for(const auto& availablePresentMode : availablePresentModes)
	{
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return availablePresentModes[0];
}

VkExtent2D vklib::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if(capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = {
	static_cast<uint32_t>(1024),
	static_cast<uint32_t>(768)
		};
		return actualExtent;
	}
}

void vklib::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		VK_CALL(vkCreateImageView, device, &createInfo, nullptr, &swapChainImageViews[i])
	}
}

void vklib::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;//VK_SAMPLE_COUNT_1_BIT
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//VK_ATTACHMENT_LOAD_OP_CLEAR
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//VK_ATTACHMENT_STORE_OP_STORE
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//VK_ATTACHMENT_LOAD_OP_DONT_CARE
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//VK_ATTACHMENT_STORE_OP_DONT_CARE
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_UNDEFINED
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;//0
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;//VK_PIPELINE_BIND_POINT_GRAPHICS
	subpass.colorAttachmentCount = 1;//0
	subpass.pColorAttachments = &colorAttachmentRef;//nullptr

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;//VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO
	renderPassInfo.attachmentCount = 1;//0
	renderPassInfo.pAttachments = &colorAttachment;//nullptr
	renderPassInfo.subpassCount = 1;//0
	renderPassInfo.pSubpasses = &subpass;//nullptr

	VK_CALL(vkCreateRenderPass, device, &renderPassInfo, nullptr, &renderPass);
}

void vklib::createGraphicsPipeline()
{
	//auto vertShaderCode = vkfile::ReadFile("shaders/vert.spv");
	//auto fragShaderCode = vkfile::ReadFile("shaders/frag.spv");
	//vkshader vert(this, "shaders/vert.spv");
	//vkshader frag(this, "shaders/frag.spv");
	vkfile::SetAssetPathRoot("assets/");
	vkshader vert(this, "vert.spv");
	vkshader frag(this, "frag.spv");


	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vert.getModule();
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = frag.getModule();
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vklibMesh* triangle = vklibMesh::DefaultCube(this);

	auto bindingDescription = vklibMesh::getBindingDescription();
	auto attributeDescriptions = vklibMesh::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)this->swapChainExtent.width;
	viewport.height = (float)this->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = this->swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;//0
	viewportState.pViewports = &viewport;//nullptr
	viewportState.scissorCount = 1;//0
	viewportState.pScissors = &scissor;//nullptr

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;//1.0f
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;//VK_CULL_MODE_NONE
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;//VK_FRONT_FACE_CLOCKWISE
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;//VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
	multisampling.sampleShadingEnable = VK_FALSE;//VK_FALSE
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//VK_SAMPLE_COUNT_1_BIT

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;//VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	colorBlendAttachment.blendEnable = VK_FALSE;//VK_FALSE

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;//VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
	colorBlending.logicOpEnable = VK_FALSE;//VK_FALSE
	colorBlending.logicOp = VK_LOGIC_OP_COPY;//VK_LOGIC_OP_COPY
	colorBlending.attachmentCount = 1;//0
	colorBlending.pAttachments = &colorBlendAttachment;//nullptr
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;//VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
	pipelineLayoutInfo.setLayoutCount = 1;//0
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;//nullptr
	//pipelineLayoutInfo.pushConstantRangeCount = 0;//0

	VK_CALL(vkCreatePipelineLayout, device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;//VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
	pipelineInfo.stageCount = 2;//0
	pipelineInfo.pStages = shaderStages;//nullptr
	pipelineInfo.pVertexInputState = &vertexInputInfo;//nullptr
	pipelineInfo.pInputAssemblyState = &inputAssembly;//nullptr
	pipelineInfo.pViewportState = &viewportState;//nullptr
	pipelineInfo.pRasterizationState = &rasterizer;//nullptr
	pipelineInfo.pMultisampleState = &multisampling;//nullptr
	pipelineInfo.pDepthStencilState = nullptr;//nullptr
	pipelineInfo.pColorBlendState = &colorBlending;//nullptr
	pipelineInfo.pDynamicState = nullptr;//nullptr
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;//0
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//VK_NULL_HANDLE

	VK_CALL(vkCreateGraphicsPipelines, device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
}

void vklib::createFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainFramebuffers.size(); ++i)
	{
		VkImageView attachments[] = { swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;//VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO
		framebufferInfo.renderPass = renderPass;//renderPass
		framebufferInfo.attachmentCount = 1;//0
		framebufferInfo.pAttachments = attachments;//nullptr
		framebufferInfo.width = swapChainExtent.width;//swapChainExtent.width
		framebufferInfo.height = swapChainExtent.height;//swapChainExtent.height
		framebufferInfo.layers = 1;//1

		VK_CALL(vkCreateFramebuffer, device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
	}
}

void vklib::createSurface()
{}

void vklib::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;//VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;//queueFamilyIndices.graphicsFamily

	VK_CALL(vkCreateCommandPool, device, &poolInfo, nullptr, &commandPool);
}

void vklib::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;//VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO
	allocInfo.commandPool = commandPool;//commandPool
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//VK_COMMAND_BUFFER_LEVEL_PRIMARY
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();//commandBuffers.size()

	VK_CALL(vkAllocateCommandBuffers, device, &allocInfo, commandBuffers.data());


	for (size_t i = 0; i < commandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
		VK_CALL(vkBeginCommandBuffer, commandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;//VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO
		renderPassInfo.renderPass = renderPass;//renderPass
		renderPassInfo.framebuffer = swapChainFramebuffers[i];//swapChainFramebuffers[i]
		renderPassInfo.renderArea.offset = { 0, 0 };//{ 0, 0 }
		renderPassInfo.renderArea.extent = swapChainExtent;//swapChainExtent

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;//1
		renderPassInfo.pClearValues = &clearColor;//&clearColor

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//VK_SUBPASS_CONTENTS_INLINE
		{
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);//VK_PIPELINE_BIND_POINT_GRAPHICS
			
			{
				vklibMesh* triangle = vklibMesh::DefaultCube(this);
				VkBuffer vertexBuffers[] = { triangle->getVertexBuffer() };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffers[i], triangle->getIndexBuffer(), 0, INDEX_TYPE2);

				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);//VK_PIPELINE_BIND_POINT_GRAPHICS
				//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);//3, 1, 0, 0
				//vkCmdDraw(commandBuffers[i], triangle->getVertexCount(), 1, 0, 0);
				vkCmdDrawIndexed(commandBuffers[i], triangle->getIndexCount(), 1, 0, 0, 0);
			}

		}
		vkCmdEndRenderPass(commandBuffers[i]);

		VK_CALL(vkEndCommandBuffer, commandBuffers[i])
	}
}


void vklib::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CALL2(vkCreateBuffer, device, &bufferInfo, nullptr, &buffer)

		VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VK_CALL2(vkAllocateMemory, device, &allocInfo, nullptr, &bufferMemory)

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void vklib::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}


void vklib::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;//VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;//VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;//VK_FENCE_CREATE_SIGNALED_BIT

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VK_CALL2(vkCreateSemaphore, device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		VK_CALL2(vkCreateSemaphore, device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
		VK_CALL2(vkCreateFence, device, &fenceInfo, nullptr, &inFlightFences[i]);
	}
}

void vklib::draw()
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);//UINT64_MAX

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);//UINT64_MAX
	//VK_CALL2(vkAcquireNextImageKHR, device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);//UINT64_MAX
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		VK_LOG_ERROR("failed to acquire swap chain image!");
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(imageIndex);


	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);//UINT64_MAX
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;//waitStages

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];//&commandBuffers[imageIndex]

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;//1
	submitInfo.pSignalSemaphores = signalSemaphores;//signalSemaphores

	vkResetFences(device, 1, &inFlightFences[currentFrame]);//inFlightFences[currentFrame]

	VK_CALL2(vkQueueSubmit, graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);//inFlightFences[currentFrame]

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;//VK_STRUCTURE_TYPE_PRESENT_INFO_KHR

	presentInfo.waitSemaphoreCount = 1;//1
	presentInfo.pWaitSemaphores = signalSemaphores;//signalSemaphores

	VkSwapchainKHR swapChains[] = { swapChain };//swapChain
	presentInfo.swapchainCount = 1;//1
	presentInfo.pSwapchains = swapChains;//swapChains

	presentInfo.pImageIndices = &imageIndex;//&imageIndex


	result = vkQueuePresentKHR(presentQueue, &presentInfo);//&presentInfo
	//VK_CALL2(vkQueuePresentKHR, presentQueue, &presentInfo);//&presentInfo
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
	{
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		VK_LOG_ERROR("failed to present swap chain image!");
		throw std::runtime_error("failed to present swap chain image!");
	}
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;//MAX_FRAMES_IN_FLIGHT

}

void vklib::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;//0
	uboLayoutBinding.descriptorCount = 1;//1
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	uboLayoutBinding.pImmutableSamplers = nullptr;//nullptr
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;//VK_SHADER_STAGE_VERTEX_BIT

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
	layoutInfo.bindingCount = 1;//0
	layoutInfo.pBindings = &uboLayoutBinding;//nullptr

	VK_CALL(vkCreateDescriptorSetLayout, device, &layoutInfo, nullptr, &descriptorSetLayout);
}

void vklib::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());//swapChainImages.size()

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO
	poolInfo.poolSizeCount = 1;//0
	poolInfo.pPoolSizes = &poolSize;//nullptr
	poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());//swapChainImages.size()

	VK_CALL(vkCreateDescriptorPool, device, &poolInfo, nullptr, &descriptorPool);
}
void vklib::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
	allocInfo.descriptorPool = descriptorPool;//descriptorPool
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());//swapChainImages.size()
	allocInfo.pSetLayouts = layouts.data();//layouts.data()

	descriptorSets.resize(swapChainImages.size());
	VK_CALL(vkAllocateDescriptorSets, device, &allocInfo, descriptorSets.data());

	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];//uniformBuffers[i]
		bufferInfo.offset = 0;//0
		bufferInfo.range = sizeof(UniformBufferObject);//sizeof(UniformBufferObject)

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET
		descriptorWrite.dstSet = descriptorSets[i];//descriptorSets[i]
		descriptorWrite.dstBinding = 0;//0
		descriptorWrite.dstArrayElement = 0;//0
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
		descriptorWrite.descriptorCount = 1;//1
		descriptorWrite.pBufferInfo = &bufferInfo;//&bufferInfo

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}	
}

void vklib::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void vklib::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

void vklib::windSizeChanged()
{
	framebufferResized = true;
}

void vklib::recreateSwapChain()
{
	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	createSwapChainList();
}

void vklib::createSwapChainList()
{
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
}

void vklib::cleanupSwapChain() {
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	for (size_t i = 0; i < uniformBuffers.size(); ++i)
	{
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void vklib::cleanup()
{
	cleanupSwapChain();
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

uint32_t vklib::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}