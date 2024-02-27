#pragma once
#include "FileSystemBase.hpp"
#include <filesystem>

class Directory;

class File : public FileSystemBase
{
private:
	std::filesystem::path m_path;

public:
	File(const std::string& path);
	~File();
	std::string GetNameWithoutExtension() const;
	std::string ReadFile();
	Directory Parent() const;
	// 检查路径是否存在

	bool Exists() const;

	// 创建目录
	static bool CreateDirectory(const std::filesystem::path& path);

	// 删除文件或目录
	static bool Remove(const std::filesystem::path& path);

	// 获取目录下的所有文件和目录
	static std::vector<std::filesystem::path> ListDirectory(const std::filesystem::path& path);

	// 读取文件内容到字符串
	static std::string ReadFile(const std::filesystem::path& path);

	// 写入字符串到文件
	static bool WriteFile(const std::filesystem::path& path, const std::string& content);

	// 复制文件
	static bool CopyFile(const std::filesystem::path& from, const std::filesystem::path& to, std::filesystem::copy_options options = std::filesystem::copy_options::overwrite_existing);

	// 移动文件
	static bool MoveFile(const std::filesystem::path& from, const std::filesystem::path& to);
};
