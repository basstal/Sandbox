#include "pch.hpp"

#include "ShaderSource.hpp"

#include "FileSystem/File.hpp"


Sandbox::ShaderSource::ShaderSource(const std::string& inFilePath, const std::string& inPreamble, VkShaderStageFlagBits inStage) :
    filePath(inFilePath), preamble(inPreamble), stage(inStage)
{
    source = File(filePath).ReadFile();
}
Sandbox::ShaderSource::ShaderSource(const File& file, const std::string& inPreamble, VkShaderStageFlagBits inStage) : ShaderSource(file.path.string(), inPreamble, inStage) {}
