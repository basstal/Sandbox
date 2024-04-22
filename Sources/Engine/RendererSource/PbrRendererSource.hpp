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

        void PushConstants(const std::shared_ptr<CommandBuffer>& inCommandBuffer) override;
        void BindPipeline(const std::shared_ptr<CommandBuffer>& inCommandBuffer) override;
        // void BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments, VkExtent2D resolution) override;

        std::vector<std::array<std::shared_ptr<Texture>, 4>> textures;
        std::vector<std::shared_ptr<UniformBuffer>>          uboLights;
        PushConstantsInfo                                    pushConstantsInfo;
        std::shared_ptr<Pipeline>                            pipeline;
    };
}  // namespace Sandbox
