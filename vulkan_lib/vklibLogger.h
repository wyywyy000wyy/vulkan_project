#pragma once
#include <sstream>
#include <iostream>
#include <string>

class vklibLogger
{
public:
	virtual void Log(const char* message) = 0;
	virtual void LogWarning(const char* message) = 0;
	virtual void LogError(const char* message) = 0;
};

void printArgs(std::stringstream& ss);


template<typename T, typename... Args>
void printArgs(std::stringstream& ss, T first, Args... args) {
	ss << first;
	printArgs(ss, args...);
}

#define FOMATION_LOG_STRING(VLOG, ...) \
{ \
	std::stringstream ss; \
	printArgs(ss, __VA_ARGS__); \
	std::string str = ss.str(); \
	if (logger) logger->VLOG(str.c_str()); \
}

#define VK_LOG(...) FOMATION_LOG_STRING(Log, __VA_ARGS__)
#define VK_LOG_WARNING(...) FOMATION_LOG_STRING(LogWarning, __VA_ARGS__)
#define VK_LOG_ERROR(...) FOMATION_LOG_STRING(LogError, __VA_ARGS__)

extern vklibLogger* logger;


#define VK_CALL(VK_CALL, ...) \
{ \
	VkResult result = VK_CALL(__VA_ARGS__); \
	if (result != VK_SUCCESS) { \
		VK_LOG_ERROR("****************************");\
		VK_LOG_ERROR("Vulkan call ",#VK_CALL," failed: ",  result); \
		VK_LOG_ERROR("****************************");\
		throw std::runtime_error("Vulkan call failed: " ); \
	} \
	VK_LOG("Vulkan call ",#VK_CALL ," success!")\
}

#define VK_CALL2(VK_CALL, ...) \
{ \
	VkResult result = VK_CALL(__VA_ARGS__); \
	if (result != VK_SUCCESS) { \
		VK_LOG_ERROR("****************************");\
		VK_LOG_ERROR("Vulkan call ",#VK_CALL," failed: ",  result); \
		VK_LOG_ERROR("****************************");\
		throw std::runtime_error("Vulkan call failed: " ); \
	} \
}