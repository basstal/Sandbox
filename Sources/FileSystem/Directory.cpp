#include "Directory.hpp"

#include "File.hpp"

Sandbox::Directory::Directory(const std::string& inPath)
{
    path = std::filesystem::path(inPath);
}

Sandbox::File Sandbox::Directory::GetFile(const std::string& name) const
{
    return File((path / name).string());
}

std::vector<Sandbox::FileSystemBase> Sandbox::Directory::DirectoryIterator()
{
    std::vector<Sandbox::FileSystemBase> result;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            result.push_back(Directory(entry.path().string()));
        }
        else
        {
            result.push_back(File(entry.path().string()));
        }
    }
    return result;
}

std::vector<Sandbox::File> Sandbox::Directory::GetFiles() const
{
    std::vector<Sandbox::File> result;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            result.push_back(File(entry.path().string()));
        }
    }
    return result;
}


std::vector<Sandbox::Directory> Sandbox::Directory::GetDirectories() const
{
    std::vector<Sandbox::Directory> result;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            result.push_back(Directory(entry.path().string()));
        }
    }
    return result;
}

std::string Sandbox::Directory::GetName() const
{
    return path.filename().string();
}

Sandbox::Directory Sandbox::Directory::GetAssetsDirectory()
{
    std::filesystem::path executablePath = std::filesystem::path::path(GetExecutablePath());
    return Directory(executablePath.parent_path().parent_path().append("Assets").string());
}
