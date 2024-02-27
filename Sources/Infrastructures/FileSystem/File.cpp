#include "File.hpp"
#include "Directory.hpp"
#include <fstream>
#include <iostream>

File::File(const std::string& path)
{
	m_path = std::filesystem::path(path);
}
File::~File()
{
}
std::string File::GetNameWithoutExtension() const
{
	std::filesystem::path resultPath = m_path.filename().replace_extension("");
	return resultPath.string();
}
std::string File::ReadFile()
{
	std::ifstream file(m_path);
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}
Directory File::Parent() const
{
	return Directory(m_path.parent_path().string());
}

bool File::Exists() const
{
	return std::filesystem::exists(m_path) && std::filesystem::is_regular_file(m_path);
}


bool File::CreateDirectory(const std::filesystem::path& path)
{
	return std::filesystem::create_directories(path);
}

bool File::Remove(const std::filesystem::path& path)
{
	return std::filesystem::remove_all(path) > 0;
}

std::vector<std::filesystem::path> File::ListDirectory(const std::filesystem::path& path)
{
	std::vector<std::filesystem::path> paths;
	if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			paths.push_back(entry.path());
		}
	}
	return paths;
}

std::string File::ReadFile(const std::filesystem::path& path)
{
	std::ifstream file(path);
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

bool File::WriteFile(const std::filesystem::path& path, const std::string& content)
{
	std::ofstream file(path);
	if (!file.is_open())
		return false;
	file << content;
	return true;
}

bool File::CopyFile(const std::filesystem::path& from, const std::filesystem::path& to, std::filesystem::copy_options options)
{
	try
	{
		std::filesystem::copy(from, to, options);
		return true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}
}

bool File::MoveFile(const std::filesystem::path& from, const std::filesystem::path& to)
{
	try
	{
		std::filesystem::rename(from, to);
		return true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}
}
