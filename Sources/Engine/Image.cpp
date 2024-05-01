#include "pch.hpp"

#include "Image.hpp"

#include "FileSystem/File.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/TypeCasting.hpp"

Sandbox::Resource::Image::Image(const std::string& path, bool flipOnLoad)
{
    stbi_set_flip_vertically_on_load(flipOnLoad);
    stbi_uc* loadPixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    // std::cout<< "load image finished" << std::endl;
    if (!loadPixels)
    {
        Logger::Fatal("failed to load texture image!");
    }
    pixels    = loadPixels;
    mipLevels = static_cast<unsigned short>(std::floor(std::log2(std::max(width, height)))) + 1;
}

Sandbox::Resource::Image::Image(const File& path) : Image(path.path.string()) {}

Sandbox::Resource::Image::~Image()
{
    if (pixels != nullptr)
    {
        stbi_image_free(pixels);
    }
}
VkExtent3D Sandbox::Resource::Image::GetExtent3D() const { return {ToUInt32(width), ToUInt32(height), 1}; }
