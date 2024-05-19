#include "pch.hpp"

#include "StbImage.hpp"

#include "FileSystem/Logger.hpp"
#include "Misc/TypeCasting.hpp"


Sandbox::Resource::StbImage::StbImage(const std::string& name, const std::vector<uint8_t>& data, bool flipOnLoad)
{
    int components = 4;
    stbi_set_flip_vertically_on_load(flipOnLoad);
    stbi_uc* loadPixels = stbi_load_from_memory(data.data(), ToInt32(data.size()), &width, &height, &channels, STBI_rgb_alpha);
    if (!loadPixels)
    {
        LOGF("Game", "failed to load texture image!")
    }
    SetData(loadPixels, width * height * components);
    mipLevels = static_cast<unsigned short>(std::floor(std::log2(std::max(width, height)))) + 1;
    stbi_image_free(loadPixels);
}
