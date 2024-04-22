#pragma once
#include "DescriptorSetKey.hpp"
#include "Misc/Hasher.hpp"
#include "Standard/Dictionary.hpp"
#include "VulkanRHI/Core/DescriptorPool.hpp"
#include "VulkanRHI/Core/DescriptorSetLayout.hpp"

namespace Sandbox
{
    class DescriptorSet;
    class DescriptorPool;
    class DescriptorSetLayout;
    class Device;

    

    class DescriptorSetCaching
    {
    public:
        DescriptorSetCaching(const std::shared_ptr<Device>& inDevice, const std::shared_ptr<DescriptorPool>& inDescriptorPool);

        std::shared_ptr<DescriptorSet> GetOrCreateDescriptorSet(const std::shared_ptr<DescriptorSetLayout>& inDescriptorSetLayout, size_t frameFlightIndex);

    private:
        Dictionary<DescriptorSetKey, std::shared_ptr<DescriptorSet>> m_descriptorSets;
        std::shared_ptr<Device>                                      m_device;
        std::shared_ptr<DescriptorPool>                              m_descriptorPool;
    };
}  // namespace Sandbox

