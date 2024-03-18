#pragma once
#include <filesystem>
#include <string>

#include "FileSystemBase.hpp"

namespace Sandbox
{
    class Directory;

    class File : public FileSystemBase
    {
    public:
        File(const std::string& inPath);

        std::string GetName() const;

        std::string GetNameWithoutExtension() const;

        std::string ReadFile();

        Directory Parent() const;

        bool Exists() const;

        void CreateDirectory();

        std::filesystem::path path;
    };
}
