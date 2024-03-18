#include "File.hpp"

#include <fstream>

#include "Directory.hpp"

Sandbox::File::File(const std::string& inPath)
{
    path = std::filesystem::path(inPath);
}

std::string Sandbox::File::GetName() const
{
    return path.filename().string();
}

std::string Sandbox::File::GetNameWithoutExtension() const
{
    std::filesystem::path resultPath = path.filename().replace_extension("");
    return resultPath.string();
}

std::string Sandbox::File::ReadFile()
{
    std::ifstream file(path);
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

Sandbox::Directory Sandbox::File::Parent() const
{
    return Directory(path.parent_path().string());
}

bool Sandbox::File::Exists() const
{
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

void Sandbox::File::CreateDirectory()
{
    std::filesystem::create_directories(path.parent_path());
}
