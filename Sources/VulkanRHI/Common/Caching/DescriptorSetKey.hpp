#pragma once
#include "Misc/Hasher.hpp"
#include "VulkanRHI/Core/DescriptorPool.hpp"
#include "VulkanRHI/Core/DescriptorSetLayout.hpp"

namespace Sandbox
{
    class DescriptorPool;
    class DescriptorSetLayout;
    struct DescriptorSetKey
    {
        std::shared_ptr<Sandbox::DescriptorPool>      descriptorPool;
        std::shared_ptr<Sandbox::DescriptorSetLayout> descriptorSetLayout;
        size_t                                        frameFlightIndex;
        bool                                          operator==(const DescriptorSetKey& other) const;
    };
}  // namespace Sandbox


namespace std
{
    template <>
    struct hash<Sandbox::DescriptorSetKey>
    {
        std::size_t operator()(const Sandbox::DescriptorSetKey& key) const noexcept
        {
            std::size_t hash = 0;
            Sandbox::HashCombined(hash, key.descriptorPool->vkDescriptorPool);
            Sandbox::HashCombined(hash, key.descriptorSetLayout->vkDescriptorSetLayout);
            Sandbox::HashCombined(hash, key.frameFlightIndex);
            return hash;
        }
    };
}  // namespace std
