#pragma once
#include <vector>
#include <string>

class vkfile
{
public:

	static std::vector<char> ReadFile(const std::string& filename);
	static void SetAssetPathRoot(const std::string& path);
protected:
	static std::string asset_path_root;
};