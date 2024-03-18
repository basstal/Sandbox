#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "FileSystem/File.hpp"
#include "FileSystem/Logger.hpp"

Sandbox::Resource::Image::Image(const std::string& path)
{
    stbi_uc* loadPixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    //std::cout<< "load image finished" << std::endl;
    if (!loadPixels)
    {
        Logger::Fatal("failed to load texture image!");
    }
    pixels = loadPixels;
    mipLevels = static_cast<unsigned short>(std::floor(std::log2(std::max(width, height)))) + 1;
}

Sandbox::Resource::Image::Image(const File& path):
    Image(path.path.string())
{
}

Sandbox::Resource::Image::~Image()
{
    if (pixels != nullptr)
    {
        stbi_image_free(pixels);
    }
}
