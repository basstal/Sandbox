#pragma once
#include <filesystem>
#include <string>

#include "FileSystemBase.hpp"

namespace Sandbox
{
    class String;
    class Directory;

    class File : public FileSystemBase
    {
    public:
        File(const std::string& inPath);
        File(const String& inPath);

        std::string GetName() const;

        std::string GetNameWithoutExtension() const;

        std::string GetExtension() const;

        std::string ReadContent() const;

        std::vector<uint8_t> ReadBytes() const ;

        Directory Parent() const;

        bool Exists() const;

        void CreateDirectory() const;

        std::string GetAssetPath() const;

        std::filesystem::path path;
    };
}  // namespace Sandbox
