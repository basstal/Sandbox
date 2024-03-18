#pragma once

#include <vulkan/vulkan.hpp>


namespace Sandbox
{
    class PipelineLayout;
    class ShaderModule;
    class RenderPass;
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
    };

    struct InputAssemblyState
    {
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    };

    struct RasterizationState
    {
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    };

    struct DepthStencilState
    {
        VkBool32 depthTestEnable = VK_TRUE;
        VkBool32 depthWriteEnable = VK_TRUE;
    };

    struct PushConstantsInfo
    {
        uint32_t size = 0;
        void* data = nullptr;
        VkShaderStageFlagBits stage = VK_SHADER_STAGE_ALL;
    };

    class PipelineState
    {
    public:
        PipelineState(const std::vector<std::shared_ptr<ShaderModule>>& shaderModules, const std::shared_ptr<RenderPass>& renderPass,
                      const std::shared_ptr<PipelineLayout>& pipelineLayout);

        ~PipelineState() = default;

        InputAssemblyState inputAssemblyState;
        VertexInputState vertexInputState;
        RasterizationState rasterizationState;
        DepthStencilState depthStencilState;

        PushConstantsInfo pushConstantsInfo;

        std::vector<std::shared_ptr<ShaderModule>> shaderModules;
        std::shared_ptr<PipelineLayout> pipelineLayout;
        std::shared_ptr<RenderPass> renderPass;
    };
}
