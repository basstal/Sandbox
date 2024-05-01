#include "pch.hpp"

#include "KtxImage.hpp"

#include "FileSystem/Logger.hpp"


Sandbox::Resource::KtxImage::KtxImage(const std::string& inName, const std::vector<uint8_t>& data) : Image("TODO")
{
    auto data_buffer = reinterpret_cast<const ktx_uint8_t*>(data.data());
    auto data_size   = static_cast<ktx_size_t>(data.size());

    ktxTexture* texture;
    ktxResult   result = ktxTexture_CreateFromMemory(data_buffer, data_size, KTX_TEXTURE_CREATE_NO_FLAGS, &texture);
    if (result != KTX_SUCCESS)
    {
        LOGF("Engine", "Error loading KTX texture: {}", inName)
    }

    // Load the image data into a vector if not already loaded
    if (!texture->pData)
    {
        std::vector<uint8_t> image_data(texture->dataSize);
        result = ktxTexture_LoadImageData(texture, image_data.data(), texture->dataSize);
        if (result != KTX_SUCCESS)
        {
            ktxTexture_Destroy(texture);
            LOGF("Engine", "Failed to load KTX image data:  {}", inName)
        }
    }

    // Set basic texture properties
    width  = static_cast<uint32_t>(texture->baseWidth);
    height = static_cast<uint32_t>(texture->baseHeight);
    depth  = texture->baseDepth;
    format = ktxTexture_GetVkFormat(texture);

    ktxTexture_Destroy(texture);
}
