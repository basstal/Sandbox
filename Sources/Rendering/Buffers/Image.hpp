#pragma once
#include <memory>
#include <vulkan/vulkan.h>

class Device;
class Buffer;

class Image
{
    /**
     * @brief image 所在的 vulkan 设备
     */
    std::shared_ptr<Device> m_device;

public:
    VkDeviceMemory vkDeviceMemory;

    /**
     * @brief vulkan image 对象
     */
    VkImage vkImage;


    Image(const std::shared_ptr<Device>& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
          VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool isCubeMap);

    /**
     * @brief 创建 image view
     * @param inVkDevice vulkan device 对象
     * @param inVkImage vulkan image 对象
     * @param format image 格式
     * @param aspectFlags image aspect flags
     * @param mipLevels mipmap levels
     * @param isCubeMap 是否是立方体贴图
     * @return image view 对象
     */
    static VkImageView CreateImageView(VkDevice inVkDevice, VkImage inVkImage, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool isCubeMap);
};
