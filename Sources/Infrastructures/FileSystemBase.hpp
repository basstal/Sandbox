#pragma once
#include <string>
#include <vector>

class FileSystemBase
{
public:
	static std::string getExecutablePath();
	static std::string getBinariesDir();
	static std::string getAssetsDir();
	static std::string getSettingsDir();
	static std::vector<char> readFile(const std::string& filename);
	static void addDllSearchPath(std::string inSearchPath);
	static bool fileExists(const std::string& string);
};
