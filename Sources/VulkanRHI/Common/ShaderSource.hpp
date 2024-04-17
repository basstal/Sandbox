#pragma once
#include <string>

namespace Sandbox
{
    class File;

    class ShaderSource
    {
    public:
        ShaderSource(const std::string& inFilePath, const std::string& inPreamble, VkShaderStageFlagBits inStage);
        ShaderSource(const File& file, const std::string& inPreamble, VkShaderStageFlagBits inStage);

        std::string           filePath;
        std::string           source;
        std::string           preamble;
        VkShaderStageFlagBits stage;
    };
}  // namespace Sandbox
