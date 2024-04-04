#pragma once
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "VulkanRHI/Common/Macros.hpp"

namespace Sandbox
{
    class DescriptorPool;
    class Device;
    class DescriptorSetLayout;

    class DescriptorSet
    {
    public:
        DescriptorSet(const std::shared_ptr<Device>& device, const std::shared_ptr<DescriptorPool>& descriptorPool,
                      const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout, const BindingMap<VkDescriptorBufferInfo>& inBufferInfoMapping,
                      const BindingMap<VkDescriptorImageInfo>& inImageInfoMapping);

        ~DescriptorSet();

        void Cleanup();

        void Allocate(const std::shared_ptr<DescriptorPool>& descriptorPool, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout);

        void Prepare(const std::map<uint32_t, std::vector<VkDescriptorBufferInfo>>& inBufferInfoMapping,
                     const std::map<uint32_t, std::vector<VkDescriptorImageInfo>>& inImageInfoMapping, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout);

        void Update();

        VkDescriptorSet vkDescriptorSet;
        // The list of write operations for the descriptor set
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

    private:
        std::map<uint32_t, std::vector<VkDescriptorImageInfo>>  m_imageInfoMapping;
        std::map<uint32_t, std::vector<VkDescriptorBufferInfo>> m_bufferInfoMapping;
        std::shared_ptr<Device>                                 m_device;
        std::shared_ptr<DescriptorPool>                         m_descriptorPool;
        bool                                                    m_cleaned = false;
        std::unordered_map<uint32_t, uint64_t>                  m_updatedBindings;
    };
}  // namespace Sandbox
