#pragma once
#include <filesystem>

#include "FileSystemBase.hpp"

class File;

class Directory : public FileSystemBase
{
private:
	std::filesystem::path m_path;

public:
	Directory(const std::string& path);
	~Directory();
	File GetFile(const std::string& name) const;
};
