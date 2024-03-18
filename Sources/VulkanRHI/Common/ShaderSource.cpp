#include "ShaderSource.hpp"

#include "FileSystem/File.hpp"

Sandbox::ShaderSource::ShaderSource(const std::string& filePath):
    filePath(filePath)
{
    source = File(filePath).ReadFile();
}

Sandbox::ShaderSource::ShaderSource(const File& file):
    ShaderSource(file.path.string())
{
}
