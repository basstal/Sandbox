#pragma once
#include <string>
#include "Shared.hpp"

class INFRASTRUCTURES_API FileSystemBase
{
public:
	static std::string getExecutablePath();
	static std::string getBinariesDir();
	static std::string getAssetsDir();
	static std::vector<char> readFile(const std::string& filename);
	static void addDllSearchPath(std::string inSearchPath);
};
