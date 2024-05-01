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
        Image(const std::string& path, bool flipOnLoad = false);
        Image(const File& path);

        ~Image();

        VkExtent3D GetExtent3D() const;

        int            width     = 0;
        int            height    = 0;
        int            channels  = 0;
        unsigned char* pixels    = nullptr;
        unsigned short mipLevels = 1;
    };
}  // namespace Sandbox::Resource
