#include "pch.hpp"

#include "Image.hpp"

#include "FileSystem/File.hpp"
#include "KtxImage.hpp"
#include "Misc/TypeCasting.hpp"
#include "StbImage.hpp"


std::shared_ptr<Sandbox::Resource::Image> Sandbox::Resource::Image::Load(const File& file, ContentType contentType)
{
    std::shared_ptr<Image> image = nullptr;

    auto data = file.ReadBytes();

    // Get extension
    auto extension = file.GetExtension();

    if (extension == ".png" || extension == ".jpg")
    {
        image = std::make_shared<StbImage>(file.GetName(), data, false);
    }
    else if (extension == ".astc")
    {
        // TODO:
        // image = std::make_unique<Astc>(name, data);
    }
    else if (extension == ".ktx" || extension == ".ktx2")
    {
        image = std::make_shared<KtxImage>(file.GetName(), data);
    }

    return image;
}

std::shared_ptr<Sandbox::Resource::Image> Sandbox::Resource::Image::Load(const std::string& path, ContentType contentType)
{
    auto file = File(path);
    return Load(file, contentType);
}
VkExtent3D Sandbox::Resource::Image::GetExtent3D() const { return {ToUInt32(width), ToUInt32(height), 1}; }

void Sandbox::Resource::Image::SetData(const uint8_t* raw_data, size_t size)
{
    assert(pixels.empty() && "Image data already set");
    pixels = {raw_data, raw_data + size};
}

std::vector<uint8_t>& Sandbox::Resource::Image::GetMutableData() { return pixels; }
