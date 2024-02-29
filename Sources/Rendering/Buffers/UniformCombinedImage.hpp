#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>


namespace GameCore
{
    class Image;
}

class Image;
class Sampler;
class Device;

/**
 * \brief 统一图像采样器类
 */
class UniformCombinedImage
{
    /**
     * \brief 是否已清理
     */
    bool m_cleaned = false;

    /**
     * \brief vulkan 设备对象
     */
    std::shared_ptr<Device> m_device;

public:
    /**
     * \brief 采样器
     */
    std::shared_ptr<Sampler> sampler;
    /**
     * \brief 图像
     */
    std::vector<std::shared_ptr<Image>> images;

    /**
     * \brief 构造函数
     * \param device 设备
     * \param samplerAddressMode 采样器地址模式
     */
    UniformCombinedImage(const std::shared_ptr<Device>& device, VkSamplerAddressMode samplerAddressMode);


    /**
     * \brief 析构函数
     */
    ~UniformCombinedImage();


    /**
     * \brief 添加图像
     * \param width 图像宽度
     * \param height 图像高度
     * \param mipLevels mipmap levels
     * \param numSamples 采样数
     * \param format 图像格式
     * \param tiling 图像 tiling
     * \param usage 图像 usage
     * \param properties 内存属性
     * \param isCubeMap 是否是立方体贴图
     * \param aspectFlags imageView aspect flags
     */
    void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
                     VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool isCubeMap, VkImageAspectFlags aspectFlags);

    /**
     * \brief 添加图像
     * \param image 图像
     */
    void AddImage(const std::shared_ptr<Image>& image);

    /**
     * \brief 创建描述符图像信息结构体
     */
    std::vector<VkDescriptorImageInfo> CreateDescriptorImageInfos();

    /**
     * \brief 创建更新描述符集结构体
     */
    VkWriteDescriptorSet CreateWriteDescriptorSet(uint32_t binding, VkDescriptorSet descriptorSet, const std::vector<VkDescriptorImageInfo>& descriptorImageInfos);

    /**
     * \brief 清理资源
     */
    void Cleanup();
};
