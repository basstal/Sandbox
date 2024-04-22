#pragma once
#include <vulkan/vulkan_core.h>
#include "Misc/Hasher.hpp"
#include "VulkanRHI/Common/Caching/ResourceCaching.hpp"

namespace Sandbox
{
    class WriteDescriptorSet
    {
    public:
        VkWriteDescriptorSet vkWriteDescriptorSet;
        bool                 operator==(const WriteDescriptorSet& other) const;
    };
}  // namespace Sandbox


namespace std
{
    template <>
    struct hash<Sandbox::WriteDescriptorSet>
    {
        size_t operator()(const Sandbox::WriteDescriptorSet& writeDescriptorSet) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, writeDescriptorSet.vkWriteDescriptorSet);
            return result;
        }
    };
}  // namespace std
