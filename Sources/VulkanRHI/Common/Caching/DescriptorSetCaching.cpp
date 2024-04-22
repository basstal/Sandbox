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
std::shared_ptr<Sandbox::DescriptorSet> Sandbox::DescriptorSetCaching::GetOrCreateDescriptorSet(const std::shared_ptr<DescriptorSetLayout>& inDescriptorSetLayout,
                                                                                                size_t                                      frameFlightIndex)
{
    // uint64_t hash;
    // HashCombined(hash, m_descriptorPool->vkDescriptorPool);
    // HashCombined(hash, inDescriptorSetLayout->vkDescriptorSetLayout);
    // HashCombined(hash, frameFlightIndex);
    auto key = DescriptorSetKey{
        .descriptorPool      = m_descriptorPool,
        .descriptorSetLayout = inDescriptorSetLayout,
        .frameFlightIndex    = frameFlightIndex,
    };
    if (m_descriptorSets.contains(key))
    {
        return m_descriptorSets.at(key);
    }
    auto newDescriptorSet = std::make_shared<DescriptorSet>(m_descriptorPool, inDescriptorSetLayout);
    m_descriptorSets[key] = newDescriptorSet;
    return newDescriptorSet;
}
