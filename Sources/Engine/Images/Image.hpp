#pragma once
#include <string>

namespace Sandbox
{
    class File;
}

namespace Sandbox::Resource
{
    /**
     * @brief Type of content held in image.
     * This helps to steer the image loaders when deciding what the format should be.
     * Some image containers don't know whether the data they contain is sRGB or not.
     * Since most applications save color images in sRGB, knowing that an image
     * contains color data helps us to better guess its format when unknown.
     */
    enum ContentType
    {
        Unknown,
        Color,
        Other
    };

    class Image
    {
    public:
        virtual ~Image() {}

        static std::shared_ptr<Sandbox::Resource::Image> Load(const File& file, ContentType contentType);

        static std::shared_ptr<Image> Load(const std::string& path, ContentType contentType);

        VkExtent3D GetExtent3D() const;
        void                  SetData(const uint8_t* raw_data, size_t size);
        std::vector<uint8_t>& GetMutableData();


        int                  width    = 0;
        int                  height   = 0;
        int                  channels = 0;
        std::vector<uint8_t> pixels;
        unsigned short       mipLevels = 1;

    protected:
        Image() {}
    };
}  // namespace Sandbox::Resource
