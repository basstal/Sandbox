#pragma once
#include <string>

#include "Misc/Hasher.hpp"

namespace Sandbox
{
    class File;

    class ShaderSource
    {
    public:
        ShaderSource(const std::string& inFilePath, const std::string& inPreamble);
        ShaderSource(const File& file, const std::string& inPreamble);

        bool        operator==(const ShaderSource&) const = default;
        void        Reload();
        std::string filePath;
        std::string source;
        std::string preamble;
    };
}  // namespace Sandbox

namespace std
{
    template <>
    struct hash<Sandbox::ShaderSource>
    {
        size_t operator()(const Sandbox::ShaderSource& shaderSource) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, shaderSource.filePath);
            Sandbox::HashCombined(result, shaderSource.source);
            Sandbox::HashCombined(result, shaderSource.preamble);
            // Sandbox::HashCombined(result, shaderSource.stage);
            return result;
        }
    };
}  // namespace std
