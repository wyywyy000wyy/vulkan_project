#include "vkfile.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "vklibLogger.h"

std::string vkfile::asset_path_root = "";

void vkfile::SetAssetPathRoot(const std::string& path)
{
	asset_path_root = path;
}

std::vector<char> vkfile::ReadFile(const std::string& filename)
{
	std::string full_path = asset_path_root + filename;
	std::ifstream file(full_path, std::ios::ate | std::ios::binary);

	std::filesystem::path currentPath = std::filesystem::current_path();
	VK_LOG(std::filesystem::current_path())

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file: " + full_path);
	}

	size_t file_size = (size_t)file.tellg();
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}