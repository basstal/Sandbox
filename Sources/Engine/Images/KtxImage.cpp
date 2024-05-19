#include "pch.hpp"

#include "KtxImage.hpp"

#include "FileSystem/Logger.hpp"


Sandbox::Resource::KtxImage::KtxImage(const std::string& inName, const std::vector<uint8_t>& data)
{
    auto dataBuffer = reinterpret_cast<const ktx_uint8_t*>(data.data());
    auto dataSize   = static_cast<ktx_size_t>(data.size());


    ktxTexture* texture;
    ktxResult   result = ktxTexture_CreateFromMemory(dataBuffer, dataSize, KTX_TEXTURE_CREATE_NO_FLAGS, &texture);
    if (result != KTX_SUCCESS)
    {
        LOGF("Engine", "Error loading KTX texture: {}", inName)
    }


    // Load the image data into a vector if not already loaded
    if (!texture->pData)
    {
        // Load
        auto& mutableData = GetMutableData();
        auto  size        = texture->dataSize;
        mutableData.resize(size);
        auto loadDataResult = ktxTexture_LoadImageData(texture, mutableData.data(), size);
        if (loadDataResult != KTX_SUCCESS)
        {
            ktxTexture_Destroy(texture);
            LOGF("Engine", "Failed to load KTX image data:  {}", inName)
        }
    }
    else
    {
        SetData(texture->pData, texture->dataSize);
    }

    // Set basic texture properties
    width  = static_cast<uint32_t>(texture->baseWidth);
    height = static_cast<uint32_t>(texture->baseHeight);
    depth  = texture->baseDepth;
    format = ktxTexture_GetVkFormat(texture);

    ktxTexture_Destroy(texture);
}
