#pragma once
#include <string>

namespace Sandbox
{
    class FileSystemBase
    {
    public:
        static std::string GetExecutablePath();
    };
}
