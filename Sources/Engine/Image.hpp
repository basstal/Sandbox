#pragma once
#include <string>

namespace Sandbox
{
    class File;
}

namespace Sandbox::Resource
{
    class Image
    {
    public:
        Image(const std::string& path);

        Image(const File& path);

        ~Image();

        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* pixels = nullptr;
        unsigned short mipLevels = 1;
    };
}
