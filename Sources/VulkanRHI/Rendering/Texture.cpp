#include "pch.hpp"

#include "Texture.hpp"

#include "FileSystem/Logger.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Sampler.hpp"

Sandbox::Texture::Texture(std::shared_ptr<Image>&& inImage, VkSamplerAddressMode samplerAddressMode) : Texture(inImage->m_device, samplerAddressMode)
{
    image     = std::move(inImage);
    imageView = std::make_shared<ImageView>(image, VK_IMAGE_VIEW_TYPE_2D, image->format);
}

Sandbox::Texture::Texture(const std::shared_ptr<Device> device, VkSamplerAddressMode samplerAddressMode)
{
    m_device = device;
    sampler  = std::make_shared<Sampler>(m_device, samplerAddressMode);
}

Sandbox::Texture::~Texture() { Cleanup(); }

VkDescriptorImageInfo Sandbox::Texture::GetDescriptorImageInfo()
{
    if (!imageView)
    {
        LOGE("VulkanRHI", "Texture::Update: imageView is nullptr")
        return {};
    }
    VkDescriptorImageInfo descriptorImageInfo;
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfo.imageView   = imageView->vkImageView;
    descriptorImageInfo.sampler     = sampler->vkSampler;
    return descriptorImageInfo;
}

void Sandbox::Texture::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    sampler->Cleanup();
    if (imageView)
    {
        imageView->Cleanup();
    }
    if (image)
    {
        image->Cleanup();
    }
    m_cleaned = true;
}
