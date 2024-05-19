#include "pch.hpp"

#include "ShaderSource.hpp"

#include "FileSystem/File.hpp"


Sandbox::ShaderSource::ShaderSource(const std::string& inFilePath, const std::string& inPreamble) : filePath(inFilePath), preamble(inPreamble)
{
    source = File(filePath).ReadContent();
}
Sandbox::ShaderSource::ShaderSource(const File& file, const std::string& inPreamble) : ShaderSource(file.path.string(), inPreamble) {}

void Sandbox::ShaderSource::Reload() { source = File(filePath).ReadContent(); }
