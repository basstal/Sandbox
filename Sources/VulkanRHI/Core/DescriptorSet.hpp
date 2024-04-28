#pragma once
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "Standard/Dictionary.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "WriteDescriptorSet.hpp"

namespace Sandbox
{
    class DescriptorPool;
    class Device;
    class DescriptorSetLayout;
    class DescriptorSetCaching;

    class DescriptorSet
    {
        friend class DescriptorSetCaching;

    public:
        DescriptorSet(const std::shared_ptr<DescriptorPool>& descriptorPool, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout);

        ~DescriptorSet();

        void Cleanup();

        void Allocate(const std::shared_ptr<DescriptorPool>& descriptorPool, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout);

        void BindBufferInfoMapping(const BindingMap<VkDescriptorBufferInfo>& inBufferInfoMapping, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout);

        void BindImageInfoMapping(const std::map<uint32_t, std::vector<VkDescriptorImageInfo>>& inImageInfoMapping,
                                  const std::shared_ptr<DescriptorSetLayout>&                   descriptorSetLayout);

        void Update();

        VkDescriptorSet vkDescriptorSet;
        // The list of write operations for the descriptor set
        std::vector<WriteDescriptorSet> writeDescriptorSets;
        std::vector<WriteDescriptorSet> writeDescriptorSetsImage;

    private:
        std::map<uint32_t, std::vector<VkDescriptorImageInfo>>  m_imageInfoMapping;
        std::map<uint32_t, std::vector<VkDescriptorBufferInfo>> m_bufferInfoMapping;
        std::shared_ptr<Device>                                 m_device;
        std::shared_ptr<DescriptorPool>                         m_descriptorPool;
        bool                                                    m_cleaned = false;
        Dictionary<WriteDescriptorSet, uint32_t>                m_updatedBindings;
    };
}  // namespace Sandbox
