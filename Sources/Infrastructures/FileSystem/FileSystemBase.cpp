#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "Logger.hpp"
#ifdef _WIN64
#include <windows.h>
#include "FileSystemBase.hpp"

std::string LPWSTRToString(WCHAR* lpwstr)
{
	if (lpwstr == nullptr)
		return std::string();

	// 获取所需的字符数（不包括终止空字符）
	int len = WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, nullptr, 0, nullptr, nullptr);

	// 分配足够的空间
	std::string str(len, '\0');

	// 实际进行转换
	WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, &str[0], len, nullptr, nullptr);

	// 返回转换后的字符串（删除最后的 '\0'）
	return str.substr(0, len - 1);
}
std::string FileSystemBase::getExecutablePath()
{
#ifdef UNICODE
	WCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	return LPWSTRToString(path);
#else
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	return std::string(path);
#endif
}

void FileSystemBase::addDllSearchPath(std::string inSearchPath)
{
	std::cout << inSearchPath << '\n';
	SetDllDirectoryA(inSearchPath.c_str());
}
bool FileSystemBase::fileExists(const std::string& string)
{
	return std::filesystem::exists(string);
}
std::string FileSystemBase::getBinariesDir()
{
	std::filesystem::path executablePath = std::filesystem::path::path(getExecutablePath());
	return executablePath.parent_path().string();
}
std::string FileSystemBase::getAssetsDir()
{
	std::filesystem::path executablePath = std::filesystem::path::path(getExecutablePath());
	return executablePath.parent_path().parent_path().append("Assets").string();
}
std::string FileSystemBase::getSettingsDir()
{
	std::filesystem::path executablePath = std::filesystem::path::path(getExecutablePath());
	return executablePath.parent_path().parent_path().append("Assets").append("Settings").string();
}
std::string FileSystemBase::getSourceDir()
{
	// TODO: temp implementation
	std::filesystem::path executablePath = std::filesystem::path::path(getExecutablePath());
	return executablePath.parent_path().parent_path().append("Sources").string();
}

std::vector<char> FileSystemBase::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		Logger::Fatal("failed to open file!" + filename);
	}

	std::streamsize fileSize = file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
#else // not windows
std::string FileSystemBase::getExecutablePath()
{
	Logger::Fatal("getExecutablePath not implemented!");
}
std::string FileSystemBase::getBinariesDir()
{
	Logger::Fatal("getBinariesDir not implemented!");
}
std::string FileSystemBase::getAssetsDir()
{
	Logger::Fatal("getAssetsDir not implemented!");
}
void FileSystemBase::addDllSearchPath(std::string inSearchPath)
{
	Logger::Fatal("addDllSearchPath not implemented!");
}
std::vector<char> FileSystemBase::readFile(const std::string& filename)
{
	Logger::Fatal("readFile not implemented!");
}
#endif
