#pragma once

#include <vulkan/vulkan.hpp>

#include "Misc/Event.hpp"
#include "Misc/Hasher.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"


namespace Sandbox
{
    class PipelineLayout;
    class ShaderModule;
    class ShaderLinkage;
    /**
     * \brief 顶点输入状态
     */
    struct VertexInputState
    {
        /**
         * \brief 顶点输入绑定描述
         */
        std::vector<VkVertexInputBindingDescription> bindings;
        /**
         * \brief
         */
        std::vector<VkVertexInputAttributeDescription> attributes;
        bool                                           operator==(const VertexInputState& other) const
        {
            return bindings.size() == other.bindings.size() && attributes.size() == other.attributes.size() &&
                std::equal(bindings.begin(), bindings.end(), other.bindings.begin(), other.bindings.end(),
                           [](const VkVertexInputBindingDescription& lhs, const VkVertexInputBindingDescription& rhs)
                           { return lhs.binding == rhs.binding && lhs.inputRate == rhs.inputRate && lhs.stride == rhs.stride; }) &&
                std::equal(attributes.begin(), attributes.end(), other.attributes.begin(), other.attributes.end(),
                           [](const VkVertexInputAttributeDescription lhs, const VkVertexInputAttributeDescription rhs)
                           { return lhs.binding == rhs.binding && lhs.format == rhs.format && lhs.location == rhs.location && lhs.offset == rhs.offset; });
        }
    };

    struct InputAssemblyState
    {
        VkPrimitiveTopology topology                                    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        bool                operator==(const InputAssemblyState&) const = default;
    };

    struct MultisampleState
    {
        bool                  sampleShadingEnable                       = VK_TRUE;
        float                 minSampleShading                          = .2f;  // Optional
        VkSampleCountFlagBits rasterizationSamples                      = VK_SAMPLE_COUNT_1_BIT;
        bool                  operator==(const MultisampleState&) const = default;
    };
    struct RasterizationState
    {
        VkPolygonMode   polygonMode                                 = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode                                    = VK_CULL_MODE_BACK_BIT;
        VkFrontFace     frontFace                                   = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        bool            operator==(const RasterizationState&) const = default;
    };

    struct DepthStencilState
    {
        VkBool32         depthTestEnable   = VK_TRUE;
        VkBool32         depthWriteEnable  = VK_TRUE;
        VkBool32         stencilTestEnable = VK_FALSE;
        VkStencilOpState front             = {};
        VkStencilOpState back              = {};
        bool             operator==(const DepthStencilState& other) const
        {
            return depthTestEnable == other.depthTestEnable && depthWriteEnable == other.depthWriteEnable && stencilTestEnable == other.stencilTestEnable &&
                front.compareMask == other.front.compareMask && front.compareOp == other.front.compareOp && front.depthFailOp == other.front.depthFailOp &&
                front.failOp == other.front.failOp && front.passOp == other.front.passOp && front.reference == other.front.reference &&
                front.writeMask == other.front.writeMask && back.compareMask == other.back.compareMask && back.compareOp == other.back.compareOp &&
                back.depthFailOp == other.back.depthFailOp && back.failOp == other.back.failOp && back.passOp == other.back.passOp && back.reference == other.back.reference &&
                back.writeMask == other.back.writeMask;
        }
    };

    struct PushConstantsInfo
    {
        uint32_t              size                                       = 0;
        void*                 data                                       = nullptr;
        VkShaderStageFlagBits stage                                      = VK_SHADER_STAGE_ALL;
        bool                  operator==(const PushConstantsInfo&) const = default;
    };

    class PipelineState : public std::enable_shared_from_this<PipelineState>
    {
    public:
        PipelineState(const std::shared_ptr<ShaderLinkage>& inShaderLinkage, const std::shared_ptr<RenderPass>& renderPass);
        PipelineState(const std::shared_ptr<ShaderLinkage>& inShaderLinkage, const std::shared_ptr<RenderPass>& renderPass, uint32_t inSubpassIndex);

        ~PipelineState() = default;

        // 复制构造函数
        PipelineState(const PipelineState& other);

        bool operator==(const PipelineState& other) const;

        InputAssemblyState inputAssemblyState;
        VertexInputState   vertexInputState;
        MultisampleState   multisampleState;
        RasterizationState rasterizationState;
        DepthStencilState  depthStencilState;

        // PushConstantsInfo pushConstantsInfo;
        uint32_t subpassIndex;

        std::shared_ptr<ShaderLinkage> shaderLinkage;
        // TODO: remove this
        // std::shared_ptr<PipelineLayout> pipelineLayout;
        std::shared_ptr<RenderPass> renderPass;
        // Event<const std::shared_ptr<PipelineState>&> onPipelineStateChanged;
    };
}  // namespace Sandbox

namespace std
{

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
    struct hash<Sandbox::MultisampleState>
    {
        size_t operator()(const Sandbox::MultisampleState& multisampleState) const noexcept
        {
            size_t result = 0;
            Sandbox::HashCombined(result, multisampleState.sampleShadingEnable);
            Sandbox::HashCombined(result, multisampleState.minSampleShading);
            Sandbox::HashCombined(result, multisampleState.rasterizationSamples);
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
            Sandbox::HashCombined(result, rasterizationState.cullMode);
            Sandbox::HashCombined(result, rasterizationState.frontFace);
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
            Sandbox::HashCombined(result, pipelineState.multisampleState);
            Sandbox::HashCombined(result, pipelineState.subpassIndex);
            Sandbox::HashCombined(result, pipelineState.shaderLinkage);
            // for (auto& shaderModule : pipelineState.shaderLinkage)
            // {
            //     Sandbox::HashCombined(result, shaderModule->vkShaderModule);
            //     Sandbox::HashCombined(result, shaderModule->vkShaderStage);
            // }
            // Sandbox::HashCombined(result, pipelineState.pipelineLayout->vkPipelineLayout);
            Sandbox::HashCombined(result, pipelineState.renderPass->vkRenderPass);
            return result;
        }
    };
}  // namespace std
