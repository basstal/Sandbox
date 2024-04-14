#pragma once
#include "RendererSource.hpp"

namespace Sandbox
{
    class UniformBuffer;
    class Texture;
    class PipelineLayout;
    class Pipeline;
    class PbrRendererSource : public RendererSource
    {
    public:
        void UpdateUniforms(uint32_t frameFlightIndex) override;
        void Cleanup() override;
        void CreatePipeline(std::shared_ptr<Renderer>& renderer) override;
        void CreatePipelineWithPreamble(std::shared_ptr<Renderer>& renderer, const std::string& preamble);
        void CreateDescriptorSets(std::shared_ptr<Renderer>& renderer) override;

        void UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer) override;

        std::vector<std::array<std::shared_ptr<Texture>, 4>> textures;
        std::vector<std::shared_ptr<UniformBuffer>>          uboLights;
    };
}  // namespace Sandbox
