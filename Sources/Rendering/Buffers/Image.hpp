#pragma once
#include <memory>
#include <vulkan/vulkan.h>

namespace GameCore
{
    class Image;
}

class Device;
class Buffer;

/**
 * @brief vulkan 图像类
 */
class Image
{
    /**
     * @brief 图像所在的 vulkan 设备
     */
    std::shared_ptr<Device> m_device;
    /**
     * @brief 是否已经清理
     */
    bool m_cleaned = false;

    /**
     * @brief 从 buffer 拷贝数据到 vulkan image
     * @param buffer 待拷贝数据的 buffer
     * @param width image 宽度
     * @param height image 高度
     */
    void CopyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height);

public:
    /**
     * @brief vulkan image 对象
     */
    VkImage vkImage;
    /**
     * @brief 图像对应的设备内存对象
     */
    VkDeviceMemory vkDeviceMemory;


    /**
     * @brief image view 对象
     */
    VkImageView vkImageView = VK_NULL_HANDLE;
    /**
     * @brief 构造函数
     * @param device vulkan 设备对象
     * @param width image 宽度
     * @param height image 高度
     * @param mipLevels mipmap levels
     * @param numSamples 采样数
     * @param format image 格式
     * @param tiling image tiling
     * @param usage image usage
     * @param properties 内存属性
     * @param isCubeMap 是否是立方体贴图
     * @param aspectFlags imageView aspect flags
     */
    Image(const std::shared_ptr<Device>& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
          VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool isCubeMap, VkImageAspectFlags aspectFlags);

    /**
     * @brief 析构函数
     */
    ~Image();

    /**
     * @brief 清理资源
     */
    void Cleanup();

    /**
     * @brief 创建 ImageView
     * @param format image 格式
     * @param aspectFlags image aspect flags
     * @param mipLevels mipmap levels
     * @param isCubeMap 是否是立方体贴图
     */
    void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool isCubeMap);

    /**
     * @brief 从 image 原图中拷贝数据到 vulkan image
     * @param inImage 原图
     * @param isHdrImage 是否是 hdr 图像
     */
    void AssignData(const std::shared_ptr<GameCore::Image>& inImage, VkFormat format, bool isHdrImage);

    /**
     * @brief 修改 image layout 状态
     * @param oldLayout 旧的 layout
     * @param newLayout 新的 layout
     * @param mipLevels mipmap levels
     */
    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);


    /**
     * @brief 生成 mipmap
     * @param imageFormat image 格式
     * @param texWidth image 宽度
     * @param texHeight image 高度
     * @param mipLevels mipmap levels
     */
    void GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    /**
     * @brief 创建 image view，一般用于对 交换链 的 Image 创建 ImageView
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
