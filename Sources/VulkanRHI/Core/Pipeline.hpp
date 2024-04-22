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
    class ShaderLinkage;

    class Pipeline
    {
        friend class PipelineCaching;
    public:
        // Pipeline(const std::shared_ptr<Device>& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules, const std::shared_ptr<RenderPass>& renderPass,
        //          const std::shared_ptr<PipelineLayout>& pipelineLayout, VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode);

        Pipeline(const std::shared_ptr<Device>& device, const std::shared_ptr<PipelineState>& pipelineState);

        // std::shared_ptr<Sandbox::PipelineState> CreatePipelineState(const std::vector<std::shared_ptr<ShaderModule>>& shaderModules,
        //                                                             const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<PipelineLayout>& pipelineLayout,
        //                                                             VkPrimitiveTopology primitiveTopology, VkPolygonMode polygonMode);

        ~Pipeline();

        void Cleanup();

        void Reload();

        std::shared_ptr<ShaderLinkage> shaderLinkage;

        VkPipeline vkPipeline;

        std::shared_ptr<PipelineLayout> pipelineLayout;

    private:
        std::shared_ptr<PipelineState> m_pipelineState;

        void CreatePipeline(const std::shared_ptr<PipelineState>& inPipelineState);

        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
