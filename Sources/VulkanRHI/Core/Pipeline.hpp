#pragma once
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"


namespace Sandbox
{
    class PipelineLayout;
    class PipelineState;
    class ShaderModule;
    class Device;
    class RenderPass;

    class Pipeline
    {
    public:
        // Pipeline(const std::shared_ptr<Device>& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules, const std::shared_ptr<RenderPass>& renderPass,
        //          const std::shared_ptr<PipelineLayout>& pipelineLayout, VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode);

        Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<PipelineState>& pipelineState);

        // std::shared_ptr<Sandbox::PipelineState> CreatePipelineState(const std::vector<std::shared_ptr<ShaderModule>>& shaderModules,
        //                                                             const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<PipelineLayout>& pipelineLayout,
        //                                                             VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode);

        ~Pipeline();

        void Cleanup();

        std::vector<std::shared_ptr<ShaderModule>> shaderModules;

        VkPipeline vkPipeline;

        std::shared_ptr<PipelineLayout> pipelineLayout;
        // std::shared_ptr<PipelineState> pipelineState;

    private:
        void CreatePipeline(const std::shared_ptr<PipelineState>& inPipelineState);

        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
