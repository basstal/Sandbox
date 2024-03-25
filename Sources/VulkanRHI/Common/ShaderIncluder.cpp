#include "pch.hpp"

#include "ShaderIncluder.hpp"

#include "FileSystem/File.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/Logger.hpp"

glslang::TShader::Includer::IncludeResult *MakeResult(const std::string& inName, const std::string& inContent)
{
    uint64_t contentSize = inContent.size();
    char* content = new char[contentSize];
    std::memcpy(content, inContent.c_str(), contentSize);
    return new glslang::TShader::Includer::IncludeResult(inName, content, contentSize, content);
}

glslang::TShader::Includer::IncludeResult *Sandbox::ShaderIncluder::includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth)
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

glslang::TShader::Includer::IncludeResult *Sandbox::ShaderIncluder::includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth)
{
    File file(headerName);
    if (!file.Exists())
    {
        Logger::Fatal("File not found: " + std::string(headerName));
    }
    std::string headerContent = file.ReadFile();
    return MakeResult(headerName, headerContent);
}

void Sandbox::ShaderIncluder::releaseInclude(IncludeResult* result)
{
    if (result != nullptr)
    {
        delete[] static_cast<char*>(result->userData);
        delete result;
    }
}
