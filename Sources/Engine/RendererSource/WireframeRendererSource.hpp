#pragma once
#include "RendererSource.hpp"

namespace Sandbox
{
    class WireframeRendererSource : public RendererSource
    {
    public:
        void CreatePipeline(std::shared_ptr<Renderer>& renderer) override;
        void CreateDescriptorSets(std::shared_ptr<Renderer>& renderer) override;
        void UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer) override;
        void Cleanup() override;
        std::shared_ptr<UniformBuffer>  colorUniformBuffer;
    };
}  // namespace Sandbox
