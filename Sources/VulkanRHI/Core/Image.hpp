#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;

    class Image
    {
        friend class ImageView;
        friend class Texture;
    public:
        Image(const std::shared_ptr<Device>& device, const VkImage& image, const VkExtent3D& extent3D, VkFormat format, VkImageUsageFlags usage,
              VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

        Image(const std::shared_ptr<Device>& device, const VkExtent3D& extent3D, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
              VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, uint32_t mipLevels = 1, uint32_t arrayLayers = 1,
              VkImageCreateFlags flags = 0);


        ~Image();

        void Cleanup();

        VkImage vkImage;
        VkDeviceMemory vkDeviceMemory;

        VkImageType type;
        VkExtent3D extent;
        VkFormat format;
        VkImageUsageFlags usage;
        VkSampleCountFlagBits sampleCount;
        VkImageTiling tiling;
        uint32_t arrayLayerCount;
        VkImageSubresource subresource;

    private:
        std::shared_ptr<Device> m_device;
        bool m_cleaned = false;
    };
}
