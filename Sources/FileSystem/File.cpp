#include "pch.hpp"

#include "File.hpp"

#include "Directory.hpp"
#include "Standard/String.hpp"

Sandbox::File::File(const std::string& inPath) { path = std::filesystem::path(inPath); }
Sandbox::File::File(const String& inPath)
{
    std::string pathString = inPath.ToStdString();
    path                   = std::filesystem::path(pathString);
}

std::string Sandbox::File::GetName() const { return path.filename().string(); }

std::string Sandbox::File::GetNameWithoutExtension() const
{
    std::filesystem::path resultPath = path.filename().replace_extension("");
    return resultPath.string();
}

std::string Sandbox::File::GetExtension() const { return path.filename().extension().string(); }

std::string Sandbox::File::ReadContent() const
{
    std::ifstream file(path);
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

std::vector<uint8_t> Sandbox::File::ReadBytes() const
{
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

Sandbox::Directory Sandbox::File::Parent() const { return Directory(path.parent_path().string()); }

bool Sandbox::File::Exists() const { return std::filesystem::exists(path) && std::filesystem::is_regular_file(path); }

void Sandbox::File::CreateDirectory() const { std::filesystem::create_directories(path.parent_path()); }

std::string Sandbox::File::GetAssetPath() const
{
    // 返回从完整路径中删除项目路径以及 Assets 路径的剩下相对路径信息
    std::filesystem::path assetPath = Directory::GetAssetsDirectory().path;
    // 相对路径信息不需要开头的斜杠
    std::filesystem::path relativePath = path.lexically_relative(assetPath);
    // 将斜杠都替换为反斜杠
    if (relativePath.empty())
    {
        // 处理错误情况，例如路径不相关
        return {};
    }

    // 将路径转换为字符串
    std::string pathStr = relativePath.string();

    // 替换所有反斜杠为正斜杠
    std::ranges::replace(pathStr, '\\', '/');

    return pathStr;
}
