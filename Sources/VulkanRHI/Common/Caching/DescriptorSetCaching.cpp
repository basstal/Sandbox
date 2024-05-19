#include "pch.hpp"

#include "DescriptorSetCaching.hpp"

#include "ResourceCaching.hpp"
#include "VulkanRHI/Core/DescriptorPool.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/DescriptorSetLayout.hpp"


Sandbox::DescriptorSetCaching::DescriptorSetCaching(const std::shared_ptr<Device>& inDevice, const std::shared_ptr<DescriptorPool>& inDescriptorPool) :
    m_device(inDevice), m_descriptorPool(inDescriptorPool)
{
}

std::shared_ptr<Sandbox::DescriptorSet> Sandbox::DescriptorSetCaching::GetOrCreateDescriptorSet(const std::string&                          inName,
                                                                                                const std::shared_ptr<DescriptorSetLayout>& inDescriptorSetLayout,
                                                                                                size_t                                      frameFlightIndex)
{
    // uint64_t hash;
    // HashCombined(hash, m_descriptorPool->vkDescriptorPool);
    // HashCombined(hash, inDescriptorSetLayout->vkDescriptorSetLayout);
    // HashCombined(hash, frameFlightIndex);
    auto key = DescriptorSetKey{
        .name                = inName,
        .descriptorPool      = m_descriptorPool,
        .descriptorSetLayout = inDescriptorSetLayout,
        .frameFlightIndex    = frameFlightIndex,
    };
    if (m_descriptorSets.contains(key))
    {
        return m_descriptorSets.at(key);
    }
    m_descriptorSets[key] = CreateDescriptorSet(inDescriptorSetLayout);
    return m_descriptorSets[key];
}

std::shared_ptr<Sandbox::DescriptorSet> Sandbox::DescriptorSetCaching::CreateDescriptorSet(const std::shared_ptr<DescriptorSetLayout>& inDescriptorSetLayout)
{
    return std::make_shared<DescriptorSet>(m_descriptorPool, inDescriptorSetLayout);
}
