#include "pch.hpp"

#include "ImageView.hpp"

#include "Device.hpp"
#include "Image.hpp"
#include "FileSystem/Logger.hpp"

VkImageSubresource ReconstructSubresource(const std::shared_ptr<Sandbox::Image>& image, uint32_t overrideMipLevels)
{
    VkImageSubresource subresource{};
    subresource.mipLevel = overrideMipLevels == 0 ? image->subresource.mipLevel : overrideMipLevels;
    subresource.arrayLayer = image->subresource.arrayLayer;
    return subresource;
}

Sandbox::ImageView::ImageView(const std::shared_ptr<Device>& device, VkImage image, VkImageSubresource imageSubresource, VkImageViewType viewType, VkFormat inFormat):
    format(inFormat)
{
    assert(inFormat != VK_FORMAT_UNDEFINED && "format must be specified");
    m_device = device;
    subresourceRange.aspectMask = std::string(vk::componentName(static_cast<vk::Format>(inFormat), 0)) == "D"
                                      ? VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT
                                      : VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = imageSubresource.mipLevel;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = imageSubresource.arrayLayer;
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = inFormat;
    viewInfo.subresourceRange = subresourceRange;
    if (vkCreateImageView(m_device->vkDevice, &viewInfo, nullptr, &vkImageView) != VK_SUCCESS)
    {
        LOGF("failed to create image view!")
    }
}

Sandbox::ImageView::ImageView(const std::shared_ptr<Image>& image, VkImageViewType viewType, VkFormat inFormat, uint32_t overrideMipLevels):
    ImageView(image->m_device, image->vkImage, ReconstructSubresource(image, overrideMipLevels), viewType, inFormat == VK_FORMAT_UNDEFINED ? image->format : inFormat)
{
}


Sandbox::ImageView::~ImageView()
{
    Cleanup();
}

void Sandbox::ImageView::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkDestroyImageView(m_device->vkDevice, vkImageView, nullptr);
    m_cleaned = true;
}
