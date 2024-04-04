#pragma once
#include <filesystem>
#include <string>
namespace Sandbox
{
    class FileSystemBase
    {
    public:
        static std::filesystem::path GetProjectRoot();
    };
}  // namespace Sandbox
