#pragma once
#include <map>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <string>

class Device;

/**
 * \brief 描述符资源类
 */
class DescriptorResource
{
    /**
     * \brief 设备指针
     */
    std::shared_ptr<Device> m_device;
    /**
     * \brief 是否已经清理
     */
    bool m_cleaned = false;
    /**
     * \brief 描述符池大小
     */
    static constexpr uint32_t DESCRIPTOR_POOL_SIZE = 2048;

public:
    /**
     * \brief vulkan 描述符池
     */
    static VkDescriptorPool vkDescriptorPool;
    /**
     * \brief vulkan 描述符布局
     */
    VkDescriptorSetLayout vkDescriptorSetLayout;
    /**
     * \brief vulkan 描述符集
     */
    std::vector<VkDescriptorSet> vkDescriptorSets;
    /**
     * \brief vulkan 描述符布局绑定
     */
    std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
    /**
     * \brief vulkan 顶点输入属性描述
     */
    std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescriptions;
    /**
     * \brief vulkan 顶点输入常量
     * TODO:映射到 vkCmdPushConstants 命令所使用的内存数据？
     */
    std::vector<VkPushConstantRange> vkPushConstantRanges;
    /**
     * \brief 统一缓冲区名称到绑定点
     */
    std::map<std::string, int32_t> nameToBinding;
    /**
     * \brief vulkan 顶点输入绑定描述
     */
    VkVertexInputBindingDescription vkVertexInputBindingDescription = {};

    /**
     * \brief 构造函数
     * \param device 设备指针
     */
    DescriptorResource(const std::shared_ptr<Device>& device);

    /**
     * \brief 析构函数
     */
    ~DescriptorResource();

    /**
     * \brief 创建描述符集布局
     */
    void CreateDescriptorSetLayout();

    /**
     * \brief 创建描述符池
     * \param device 设备指针
     */
    static void CreateDescriptorPool(const std::shared_ptr<Device>& device);

    /**
     * \brief 清理描述符池
     * \param device 设备指针
     */
    static void CleanupDescriptorPool(const std::shared_ptr<Device>& device);


    void CreateDescriptorSets();

    /**
     * \brief 清理资源
     */
    void Cleanup();
};
