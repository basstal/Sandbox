#pragma once
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

class Surface;

/**
 * \brief vulkan 队列族索引信息
 */
struct QueueFamilyIndices
{
    /**
     * \brief 图形队列族索引
     */
    std::optional<uint32_t> graphicsFamily;
    /**
     * \brief 显示队列族索引
     */
    std::optional<uint32_t> presentFamily;

    /**
     * \brief 是否完整
     * \return
     */
    bool isComplete() const;
};

/**
 * \brief 交换链支持信息
 */
struct SwapchainSupportDetails
{
    /**
     * \brief 交换链能力
     */
    VkSurfaceCapabilitiesKHR capabilities;
    /**
     * \brief 交换链格式
     */
    std::vector<VkSurfaceFormatKHR> formats;
    /**
     * \brief 交换链呈现模式
     */
    std::vector<VkPresentModeKHR> presentModes;
};

/**
 * \brief vulkan 设备
 */
class Device
{
    /**
     * \brief 该设备对应的 surface 对象
     */
    std::shared_ptr<Surface> m_surface;

    /**
     * \brief 给物理设备按适应性打分，分数越高代表适应的能力越多
     */
    int RateDeviceSuitability(const VkPhysicalDevice& device) const;


    /**
     * \brief 获得物理设备可用的最大多重采样数
     * \return 多重采样数
     */
    VkSampleCountFlagBits GetMaxUsableSampleCount() const;


    /**
     * \brief 查找队列族索引信息
     * \param device 物理设备
     * \return 队列族索引信息
     */
    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device) const;

    /**
     * \brief 选择物理设备
     * \param instance vulkan 实例
     */
    void SelectPhysicalDevice(const VkInstance& instance);

    /**
     * \brief 创建逻辑设备
     */
    void CreateDevice();

    /**
     * \brief 是否已清理
     */
    bool m_cleaned = false;

public:
    /**
       * \brief 查询交换链支持信息
       * \param device 物理设备
       * \return 交换链支持信息
       */
    SwapchainSupportDetails QuerySwapchainSupport(const VkPhysicalDevice& device) const;

    /**
     * \brief 固定的 vulkan 扩展，必须被支持
     */
    static std::vector<const char*> fixedDeviceExtensions;


    /**
     * \brief vulkan 物理设备
     */
    VkPhysicalDevice vkPhysicalDevice;
    /**
     * \brief vulkan 逻辑设备
     */
    VkDevice vkDevice;
    /**
     * \brief 图形队列
     */
    VkQueue graphicsQueue;
    /**
     * \brief 显示队列
     */
    VkQueue presentQueue;
    /**
     * \brief 多重采样数
     */
    VkSampleCountFlagBits msaaSamples;
    /**
     * \brief 使用中的队列族索引信息
     */
    QueueFamilyIndices queueFamilies;

    /**
     * \brief 构造函数
     * \param instance vulkan 实例
     * \param surface surface 对象
     */
    Device(const VkInstance& instance, const std::shared_ptr<Surface>& surface);

    /**
     * \brief 析构函数
     */
    ~Device();


    /**
     * \brief 清理资源
     */
    void Cleanup();
};
