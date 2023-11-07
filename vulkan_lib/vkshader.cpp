#include "vkshader.h"
#include "vkfile.h"

vkshader::vkshader(vklib* lib, std::string path)
{
    this->lib = lib;
    std::vector<char> code = vkfile::ReadFile("shaders/" + path);
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VK_CALL(vkCreateShaderModule, lib->device, &createInfo, nullptr, &shaderModule);
}
vkshader::~vkshader()
{
    vkDestroyShaderModule(lib->device, shaderModule, nullptr);
}
