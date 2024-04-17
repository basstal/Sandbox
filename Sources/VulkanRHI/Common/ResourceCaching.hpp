#pragma once
#include <cstdint>
#include <functional>

#include "ShaderSource.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"

namespace Sandbox
{

    template <typename T>
    void HashCombined(uint64_t& hash, const T& param)
    {
        std::hash<T> hasher;
        hash = hash * 31 + hasher(param);
    }

    /**
     * TODO:存在 hash 冲突的理论可能，这里没有处理
     * @tparam T
     * @param hash
     * @param param
     */
    template <typename T>
    void HashParam(uint64_t& hash, const T& param)
    {
        HashCombined(hash, param);
    }
}  // namespace Sandbox


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

    template <>
    struct hash<Sandbox::PipelineState>
    {
        size_t operator()(const Sandbox::PipelineState& pipelineState) const noexcept
        {
            size_t result = 0;
            // 这里只关心创建 pipeline 时被作为参数的字段
            Sandbox::HashCombined(result, pipelineState.inputAssemblyState);
            Sandbox::HashCombined(result, pipelineState.vertexInputState);
            Sandbox::HashCombined(result, pipelineState.rasterizationState);
            Sandbox::HashCombined(result, pipelineState.depthStencilState);
            for (auto& shaderModule : pipelineState.shaderModules)
            {
                Sandbox::HashCombined(result, shaderModule->vkShaderModule);
                Sandbox::HashCombined(result, shaderModule->vkShaderStage);
            }
            // Sandbox::HashCombined(result, pipelineState.pipelineLayout->vkPipelineLayout);
            Sandbox::HashCombined(result, pipelineState.renderPass->vkRenderPass);
            return result;
        }
    };

    template <>
    struct hash<Sandbox::InputAssemblyState>
    {
        size_t operator()(const Sandbox::InputAssemblyState& inputAssemblyState) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, inputAssemblyState.topology);
            return result;
        }
    };

    template <>
    struct hash<Sandbox::VertexInputState>
    {
        size_t operator()(const Sandbox::VertexInputState& vertexInputState) const noexcept
        {
            size_t result = 0;
            for (auto& binding : vertexInputState.bindings)
            {
                Sandbox::HashCombined(result, binding.binding);
                Sandbox::HashCombined(result, binding.stride);
                Sandbox::HashCombined(result, binding.inputRate);
            }
            for (auto& attribute : vertexInputState.attributes)
            {
                Sandbox::HashCombined(result, attribute.location);
                Sandbox::HashCombined(result, attribute.binding);
                Sandbox::HashCombined(result, attribute.format);
                Sandbox::HashCombined(result, attribute.offset);
            }
            return result;
        }
    };

    template <>
    struct hash<Sandbox::RasterizationState>
    {
        size_t operator()(const Sandbox::RasterizationState& rasterizationState) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, rasterizationState.polygonMode);
            return result;
        }
    };

    template <>
    struct hash<Sandbox::DepthStencilState>
    {
        size_t operator()(const Sandbox::DepthStencilState& depthStencilState) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, depthStencilState.depthTestEnable);
            Sandbox::HashCombined(result, depthStencilState.depthWriteEnable);
            Sandbox::HashCombined(result, depthStencilState.stencilTestEnable);
            Sandbox::HashCombined(result, depthStencilState.front);
            Sandbox::HashCombined(result, depthStencilState.back);
            return result;
        }
    };

    template <>
    struct hash<VkStencilOpState>
    {
        size_t operator()(const VkStencilOpState& stencilOpState) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, stencilOpState.failOp);
            Sandbox::HashCombined(result, stencilOpState.passOp);
            Sandbox::HashCombined(result, stencilOpState.depthFailOp);
            Sandbox::HashCombined(result, stencilOpState.compareOp);
            Sandbox::HashCombined(result, stencilOpState.compareMask);
            Sandbox::HashCombined(result, stencilOpState.writeMask);
            Sandbox::HashCombined(result, stencilOpState.reference);
            return result;
        }
    };

    template <>
    struct hash<Sandbox::ShaderSource>
    {
        size_t operator()(const Sandbox::ShaderSource& shaderSource) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, shaderSource.filePath);
            Sandbox::HashCombined(result, shaderSource.preamble);
            // Sandbox::HashCombined(result, shaderSource.source);
            Sandbox::HashCombined(result, shaderSource.stage);
            return result;
        }
    };

}  // namespace std
