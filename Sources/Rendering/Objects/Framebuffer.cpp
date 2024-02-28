#include "Framebuffer.hpp"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "GameCore/Resources/Image.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Buffers/Image.hpp"
#include "Rendering/Buffers/Buffer.hpp"
#include "Rendering/Components/Swapchain.hpp"


Framebuffer::Framebuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, bool isCubeMap, bool isHdrImage, uint32_t width,
                         uint32_t height,
                         uint32_t mipLevels)
{
    m_device = device;
    m_commandPool = commandResource;
    vkFormat = isHdrImage ? VK_FORMAT_R16G16B16A16_SFLOAT : Swapchain::COLOR_FORMAT;
    // mipLevels = isCubeMap ? 1 : mipLevels;
    auto usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image = std::make_shared<Image>(device, width, height, mipLevels, VK_SAMPLE_COUNT_1_BIT,
                                    vkFormat,
                                    VK_IMAGE_TILING_OPTIMAL,
                                    usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    isCubeMap);
    vkImageView = Image::CreateImageView(device->vkDevice, image->vkImage, vkFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, isCubeMap);
    CreateTextureSampler(isCubeMap ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT); // TODO: 共享纹理采样器
    m_supportImageProperty.width = width;
    m_supportImageProperty.height = height;
    m_supportImageProperty.isHdrImage = isHdrImage;
    m_supportImageProperty.mipLevels = mipLevels;
}

Framebuffer::Framebuffer(const std::shared_ptr<Device>& device, VkRenderPass vkRenderPass, const std::vector<VkImageView>& attachments, VkExtent2D vkExtent2D)
{
    m_device = device;
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vkRenderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = vkExtent2D.width;
    framebufferInfo.height = vkExtent2D.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device->vkDevice, &framebufferInfo, nullptr, &vkFramebuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create framebuffer!");
    }
}

bool Framebuffer::IsSupportImageProperty(uint32_t width, uint32_t height, bool isHdrImage)
{
    if (m_supportImageProperty.width == width && m_supportImageProperty.height == height && m_supportImageProperty.isHdrImage == isHdrImage)
    {
        return true;
    }
    return false;
}

void Framebuffer::AssignImageData(const std::shared_ptr<GameCore::Image>& image)
{
    auto isHdrImage = image->IsHdr();
    if (!IsSupportImageProperty(image->Width(), image->Height(), isHdrImage))
    {
        Logger::Fatal("Image is not supported by this RenderTexture!");
    }
    auto byteSize = isHdrImage ? 8 : 4;
    VkDeviceSize imageSize = (VkDeviceSize)image->Width() * image->Height() * byteSize;
    Buffer textureStagingBuffer(m_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* data;
    vkMapMemory(m_device->vkDevice, textureStagingBuffer.vkDeviceMemory, 0, imageSize, 0, &data);
    if (isHdrImage)
    {
        memcpy(data, image->PixelsHdr(), static_cast<size_t>(imageSize));
    }
    else
    {
        memcpy(data, image->Pixels(), static_cast<size_t>(imageSize));
    }
    vkUnmapMemory(m_device->vkDevice, textureStagingBuffer.vkDeviceMemory);
    TransitionImageLayout(vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image->MipLevels());
    CopyFromBuffer(textureStagingBuffer.vkBuffer, static_cast<uint32_t>(image->Width()), static_cast<uint32_t>(image->Height()));
    GenerateMipmaps(vkFormat, image->Width(), image->Height(), image->MipLevels());
}

Framebuffer::~Framebuffer()
{
    Cleanup();
}

void Framebuffer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    // vkDestroySampler(m_device->vkDevice, vkSampler, nullptr);
    // vkDestroyImageView(m_device->vkDevice, vkImageView, nullptr);
    // vkDestroyImage(m_device->vkDevice, vkImage, nullptr);
    // vkFreeMemory(m_device->vkDevice, vkDeviceMemory, nullptr);
    m_cleaned = true;
}

void Framebuffer::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
{
    TransitionImageLayout(vkFormat, oldLayout, newLayout, m_supportImageProperty.mipLevels);
}

void Framebuffer::TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = m_commandPool->BeginSingleTimeCommands();
    VkImageMemoryBarrier barrier{};
    VkPipelineStageFlags sourceStage, destinationStage;
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image->vkImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    // else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    // {
    // 	barrier.srcAccessMask = 0;
    // 	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    //
    // 	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    // 	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    // }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
    m_commandPool->EndSingleTimeCommands(commandBuffer);
}

void Framebuffer::CopyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = m_commandPool->BeginSingleTimeCommands();
    VkBufferImageCopy region;
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image->vkImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    m_commandPool->EndSingleTimeCommands(commandBuffer);
}


void Framebuffer::GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_device->vkPhysicalDevice, imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        Logger::Fatal("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = m_commandPool->BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image->vkImage;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
                       image->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image->vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
        if (mipWidth > 1)
        {
            mipWidth /= 2;
        }
        if (mipHeight > 1)
        {
            mipHeight /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    m_commandPool->EndSingleTimeCommands(commandBuffer);
}

void Framebuffer::CreateTextureSampler(VkSamplerAddressMode samplerAddressMode)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = samplerAddressMode;
    samplerInfo.addressModeV = samplerAddressMode;
    samplerInfo.addressModeW = samplerAddressMode;
    samplerInfo.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_device->vkPhysicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
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


// void Framebuffer::CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass)
// {
//     m_colorImage = std::make_shared<Image>(m_device, vkExtent2D.width, vkExtent2D.height, 1, m_device->msaaSamples, vkFormat, VK_IMAGE_TILING_OPTIMAL,
//                                            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, false);
//     m_vkColorImageView = Image::CreateImageView(m_device->vkDevice, m_colorImage->vkImage, vkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, false);
//
//     VkFormat depthFormat = renderPass->FindDepthFormat();
//     m_depthImage = std::make_shared<Image>(m_device, vkExtent2D.width, vkExtent2D.height, 1, m_device->msaaSamples, depthFormat,
//                                            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false);
//     m_vkDepthImageView = Image::CreateImageView(m_device->vkDevice, m_depthImage->vkImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, false);
//     vkFramebuffers.resize(vkImageViews.size());
//     for (size_t i = 0; i < vkImageViews.size(); i++)
//     {
//         const std::vector<VkImageView> attachments = {
//             m_vkColorImageView,
//             vkImageViews[i],
//             m_vkDepthImageView
//         };
//
//         CreateFramebuffer(vkFramebuffers[i], renderPass->vkRenderPass, attachments, TODO);
//     }
//     m_framebufferCreated = true;
// }
