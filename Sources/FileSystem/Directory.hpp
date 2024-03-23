#pragma once
#include <filesystem>
#include <string>

#include "FileSystemBase.hpp"

namespace Sandbox
{
    class File;

    class Directory : public FileSystemBase
    {
    public:
        Directory(const std::string& inPath);

        File GetFile(const std::string& name) const;

        std::vector<Sandbox::FileSystemBase> DirectoryIterator();

        std::vector<File> GetFiles() const;

        std::vector<Sandbox::Directory> GetDirectories() const;

        std::string GetName() const;

        std::filesystem::path path;

        static Directory GetAssetsDirectory();

        static Directory GetLibraryDirectory();
    };
}
