#include "ShaderIncluder.hpp"
#include "Infrastructures/FileSystem/Directory.hpp"
#include "Infrastructures/FileSystem/File.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"

glslang::TShader::Includer::IncludeResult *MakeResult(const std::string& inName, const std::string& inContent)
{
    uint64_t contentSize = inContent.size();
    char* content = new char[contentSize];
    std::memcpy(content, inContent.c_str(), contentSize);
    return new glslang::TShader::Includer::IncludeResult(inName, content, contentSize, content);
}

glslang::TShader::Includer::IncludeResult *ShaderIncluder::includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth)
{
    File file(includerName);
    if (!file.Exists())
    {
        Logger::Fatal("File not found: " + std::string(includerName));
    }
    Directory directory = file.Parent();
    File headerFile = directory.GetFile(std::string(headerName));
    if (!headerFile.Exists())
    {
        Logger::Fatal("File not found: " + std::string(headerName));
    }
    std::string headerContent = headerFile.ReadFile();
    return MakeResult(headerName, headerContent);
}

glslang::TShader::Includer::IncludeResult *ShaderIncluder::includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth)
{
    File file(headerName);
    if (!file.Exists())
    {
        Logger::Fatal("File not found: " + std::string(headerName));
    }
    std::string headerContent = file.ReadFile();
    return MakeResult(headerName, headerContent);
}

void ShaderIncluder::releaseInclude(IncludeResult* result)
{
    if (result != nullptr)
    {
        delete[] static_cast<char*>(result->userData);
        delete result;
    }
}
