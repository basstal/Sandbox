#pragma once
#include "RendererSource.hpp"
#include "WireframeRendererSource.hpp"

namespace Sandbox
{
    class StencilRendererSource : public WireframeRendererSource
    {
    public:
        void Prepare(std::shared_ptr<Renderer>& renderer) override;
        void CreatePipeline(std::shared_ptr<Renderer>& renderer) override;
        void CustomDrawOverlay(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<DescriptorSet>& descriptorSet,
                               uint32_t frameFlightIndex, uint32_t dynamicOffsets) override;

        void CustomDrawMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<DescriptorSet>& descriptorSet,
                            uint32_t frameFlightIndex, uint32_t dynamicOffsets) override;
        std::shared_ptr<Pipeline> stencilInputLitPipeline;
        std::shared_ptr<Pipeline> stencilOutlinePipeline;

        void Cleanup() override;

    private:
        std::shared_ptr<Renderer> m_renderer;
    };
}  // namespace Sandbox
