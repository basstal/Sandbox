#pragma once
#include <string>

namespace Sandbox
{
    class File;

    class ShaderSource
    {
    public:
        ShaderSource(const std::string& filePath);
        ShaderSource(const File& file);

        std::string filePath;
        std::string source;
    };
}  // namespace Sandbox
