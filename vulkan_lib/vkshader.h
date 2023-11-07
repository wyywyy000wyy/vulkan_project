#pragma once
#include <string>
#include "vklib.h"

class vkshader
{
public:

	vkshader(vklib* lib, std::string path);
	~vkshader();

	VkShaderModule getModule() { return shaderModule; }
protected:
	VkShaderModule shaderModule;
	vklib* lib;
};