#include "Directory.hpp"
#include "File.hpp"

Directory::Directory(const std::string& path)
{
	m_path = path;
}
Directory::~Directory()
{
}
File Directory::GetFile(const std::string& name) const
{
	return File((m_path / name).string());
}
