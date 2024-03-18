#include "Sampler.hpp"

#include "Device.hpp"
#include "FileSystem/Logger.hpp"

Sandbox::Sampler::Sampler(const std::shared_ptr<Device>& device, VkSamplerAddressMode samplerAddressMode)
{
    m_device = device;

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = samplerAddressMode;
    samplerInfo.addressModeV = samplerAddressMode;
    samplerInfo.addressModeW = samplerAddressMode;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = device->GetMaxAnisotropy();
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f; // Optional
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.mipLodBias = 0.0f; // Optional
    if (vkCreateSampler(m_device->vkDevice, &samplerInfo, nullptr, &vkSampler) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create texture sampler!");
    }
}

Sandbox::Sampler::~Sampler()
{
    Cleanup();
}

void Sandbox::Sampler::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroySampler(m_device->vkDevice, vkSampler, nullptr);
    m_cleaned = true;
}
