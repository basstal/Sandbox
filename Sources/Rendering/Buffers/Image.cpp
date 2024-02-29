#include "Image.hpp"

#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/Base/Device.hpp"
#include "Rendering/Base/Properties.hpp"
#include "GameCore/Resources/Image.hpp"

#include "Rendering/Components/CommandResource.hpp"

Image::Image(const std::shared_ptr<Device>& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
             VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool isCubeMap, VkImageAspectFlags aspectFlags)
{
    m_device = device;
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = isCubeMap ? 6 : 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = numSamples;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (isCubeMap)
    {
        imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    if (vkCreateImage(device->vkDevice, &imageCreateInfo, nullptr, &vkImage) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->vkDevice, vkImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FIND_MEMORY_TYPE(device->vkPhysicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device->vkDevice, &allocInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device->vkDevice, vkImage, vkDeviceMemory, 0);

    vkImageView = CreateImageView(device->vkDevice, vkImage, format, aspectFlags, mipLevels, isCubeMap);
}

Image::~Image()
{
    Cleanup();
}

void Image::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    if (vkImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_device->vkDevice, vkImageView, nullptr);
    }
    vkDestroyImage(m_device->vkDevice, vkImage, nullptr);
    vkFreeMemory(m_device->vkDevice, vkDeviceMemory, nullptr);
    m_cleaned = true;
}


// void Image::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool isCubeMap)
// {
//     vkImageView = CreateImageView(m_device->vkDevice, vkImage, format, aspectFlags, mipLevels, isCubeMap);
// }


void Image::AssignData(const std::shared_ptr<GameCore::Image>& inImage, VkFormat format, bool isHdrImage)
{
    auto byteSize = isHdrImage ? 8 : 4;
    VkDeviceSize imageSize = (VkDeviceSize)inImage->Width() * inImage->Height() * byteSize;
    Buffer stagingBuffer(m_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* data;
    vkMapMemory(m_device->vkDevice, stagingBuffer.vkDeviceMemory, 0, imageSize, 0, &data);
    if (isHdrImage)
    {
        memcpy(data, inImage->PixelsHdr(), static_cast<size_t>(imageSize));
    }
    else
    {
        memcpy(data, inImage->Pixels(), static_cast<size_t>(imageSize));
    }
    vkUnmapMemory(m_device->vkDevice, stagingBuffer.vkDeviceMemory);
    TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, inImage->MipLevels());
    CopyFromBuffer(stagingBuffer.vkBuffer, static_cast<uint32_t>(inImage->Width()), static_cast<uint32_t>(inImage->Height()));
    GenerateMipmaps(format, inImage->Width(), inImage->Height(), inImage->MipLevels());
}

void Image::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = CommandResource::BeginSingleTimeGraphicsCommands(m_device);
    VkImageMemoryBarrier barrier{};
    VkPipelineStageFlags sourceStage{}, destinationStage{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vkImage;
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
    else
    {
        Logger::Fatal("unsupported layout transition!");
    }
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
    CommandResource::EndSingleTimeGraphicsCommands(m_device);
}

void Image::CopyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = CommandResource::BeginSingleTimeGraphicsCommands(m_device);
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
        vkImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    CommandResource::EndSingleTimeGraphicsCommands(m_device);
}

void Image::GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_device->vkPhysicalDevice, imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        Logger::Fatal("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = CommandResource::BeginSingleTimeGraphicsCommands(m_device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = vkImage;
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
                       vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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

    CommandResource::EndSingleTimeGraphicsCommands(m_device);
}

VkImageView Image::CreateImageView(VkDevice inVkDevice, VkImage inVkImage, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool isCubeMap)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = inVkImage;
    viewInfo.viewType = isCubeMap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = isCubeMap ? 6 : 1;

    VkImageView imageView;
    if (vkCreateImageView(inVkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create image view!");
    }

    return imageView;
}
