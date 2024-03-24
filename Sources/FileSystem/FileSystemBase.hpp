#pragma once
#include <string>
#include <filesystem>
namespace Sandbox
{
    class FileSystemBase
    {
    public:
        static std::filesystem::path GetProjectRoot();
    };
}
