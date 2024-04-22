#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>

namespace Sandbox
{
    class Surface;
    class Instance;

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

    class Device
    {
    public:
        Device(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Surface>& surface, const std::vector<const char*>& deviceExtensions);

        ~Device();

        void Cleanup();

        void SelectPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface, const std::vector<const char*>& deviceExtensions);

        void CreateLogicalDevice(const VkSurfaceKHR& surface, const std::vector<const char*>& deviceExtensions);

        int RateDeviceSuitability(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const std::vector<const char*>& deviceExtensions) const;

        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;

        bool IsDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<const char*>& deviceExtensions) const;

        SwapchainSupportDetails QuerySwapchainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;

        uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

        uint32_t GetMaxStorageBuffer() const;

        uint32_t GetMaxUniformBuffer() const;

        VkSampleCountFlagBits GetMaxUsableSampleCount() const;
        VkSampleCountFlagBits GetUsableSampleCount(VkSampleCountFlagBits targetSampleCount)const;

        uint32_t GetMaxPushConstantsSize() const;

        float GetMaxAnisotropy() const;

        VkDeviceSize GetMinUniformBufferOffsetAlignment(VkDeviceSize& dynamicAlignment);

        VkPhysicalDevice vkPhysicalDevice;
        VkDevice         vkDevice;
        /**
         * \brief 图形队列
         */
        VkQueue graphicsQueue;
        /**
         * \brief 显示队列
         */
        VkQueue presentQueue;

        QueueFamilyIndices queueFamilyIndices;

    private:
        bool m_cleaned = false;
    };
}
