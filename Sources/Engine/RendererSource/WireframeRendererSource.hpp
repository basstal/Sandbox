#pragma once
#include "RendererSource.hpp"

namespace Sandbox
{
    class WireframeRendererSource : public RendererSource
    {
    public:
        void                           CreatePipeline(std::shared_ptr<Renderer>& renderer) override;
        void                           CreateDescriptorSets(std::shared_ptr<Renderer>& renderer) override;
        void                           UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer) override;
        void                           Cleanup() override;
        void BindPipeline(const std::shared_ptr<CommandBuffer>& inCommandBuffer) override;
        std::shared_ptr<UniformBuffer> colorUniformBuffer;
        std::shared_ptr<Pipeline>      pipeline;
    };
}  // namespace Sandbox
