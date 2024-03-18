#pragma once
#include <cstdint>
#include <functional>

#include "VulkanRHI/Core/DescriptorSet.hpp"

namespace Sandbox
{
    template <typename T>
    void HashCombined(uint64_t& hash, const T& param)
    {
        std::hash<T> hasher;
        hash = hash * 31 + hasher(param);
    }

    template <typename T>
    void HashParam(uint64_t& hash, const T& param)
    {
        HashCombined(hash, param);
    }
}


namespace std
{
    template <>
    struct hash<VkWriteDescriptorSet>
    {
        size_t operator()(const VkWriteDescriptorSet& writeDescriptorSet) const noexcept
        {
            size_t result = 0;

            Sandbox::HashCombined(result, writeDescriptorSet.dstSet);
            Sandbox::HashCombined(result, writeDescriptorSet.dstBinding);
            Sandbox::HashCombined(result, writeDescriptorSet.dstArrayElement);
            Sandbox::HashCombined(result, writeDescriptorSet.descriptorCount);
            Sandbox::HashCombined(result, writeDescriptorSet.descriptorType);

            switch (writeDescriptorSet.descriptorType)
            {
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                for (uint32_t i = 0; i < writeDescriptorSet.descriptorCount; ++i)
                {
                    Sandbox::HashCombined(result, writeDescriptorSet.pImageInfo[i]);
                }
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                for (uint32_t i = 0; i < writeDescriptorSet.descriptorCount; ++i)
                {
                    Sandbox::HashCombined(result, writeDescriptorSet.pTexelBufferView[i]);
                }
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                for (uint32_t i = 0; i < writeDescriptorSet.descriptorCount; ++i)
                {
                    Sandbox::HashCombined(result, writeDescriptorSet.pBufferInfo[i]);
                }
                break;
            case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
            case VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM:
            case VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM:
            case VK_DESCRIPTOR_TYPE_MUTABLE_EXT:
            case VK_DESCRIPTOR_TYPE_MAX_ENUM:
                // dont care
                break;
            }
            return result;
        }
    };

    template <>
    struct hash<VkDescriptorImageInfo>
    {
        size_t operator()(const VkDescriptorImageInfo& descriptorImageInfo) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, descriptorImageInfo.sampler);
            Sandbox::HashCombined(result, static_cast<std::underlying_type<VkImageLayout>::type>(descriptorImageInfo.imageLayout));
            Sandbox::HashCombined(result, descriptorImageInfo.imageView);
            return result;
        }
    };

    template <>
    struct hash<VkDescriptorBufferInfo>
    {
        size_t operator()(const VkDescriptorBufferInfo& descriptorBufferInfo) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, descriptorBufferInfo.buffer);
            Sandbox::HashCombined(result, descriptorBufferInfo.offset);
            Sandbox::HashCombined(result, descriptorBufferInfo.range);
            return result;
        }
    };
}
