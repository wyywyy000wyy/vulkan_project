#if __ANDROID__
#include <vulkan_wrapper.h>
#else
#include <vulkan/vulkan.h>
#endif
#include "vklibLogger.h"

#define VERTEX_TYPE float
#define INDEX_TYPE uint32_t
#define INDEX_TYPE2 VK_INDEX_TYPE_UINT32