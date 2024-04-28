#pragma once
#include "RendererSource/RendererSource.hpp"

namespace Sandbox
{
    class Renderer;
    class CommandBuffer;
    class RenderPass;
    class RenderAttachments;
    class RenderTarget;
    class DescriptorSet;
    class ShaderSource;
    class ShaderLinkage;
    class PipelineState;
    class Pipeline;
    class Sampler;

    class PostProcess
    {
    public:
        PostProcess();
        PostProcess(const std::vector<std::shared_ptr<ShaderSource>>& shaderSource);

        ~PostProcess();

        void Prepare(const std::shared_ptr<Renderer>& renderer);
        bool IsPrepared();

        void Apply(const std::shared_ptr<Sandbox::CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& inRenderAttachments, VkExtent2D resolution,
                   const std::shared_ptr<RendererSource>& rendererSource);

        void Cleanup();
        void OnRecreateSwapchain();
        void AddPostProcess(const std::shared_ptr<ShaderSource>& shaderSource);

        void RemovePostProcess(const std::shared_ptr<ShaderSource>& shaderSource);

        std::vector<std::shared_ptr<RenderPass>> renderpasses;

        std::vector<std::vector<std::shared_ptr<RenderAttachments>>> renderAttachments;
        std::vector<std::vector<std::shared_ptr<RenderTarget>>>      renderTargets;

        std::vector<std::shared_ptr<ShaderLinkage>> shaderLinkages;
        std::vector<std::shared_ptr<PipelineState>> pipelineStates;

        std::vector<std::shared_ptr<Pipeline>> pipelines;
        std::shared_ptr<Sampler>               sampler;

        std::vector<std::vector<std::shared_ptr<DescriptorSet>>> descriptorSets;
        std::vector<std::shared_ptr<Image>>                      renderpassImages;
        std::vector<std::shared_ptr<ImageView>>                  renderpassImageViews;

    private:
        std::vector<std::shared_ptr<ShaderSource>> m_postProcessShaderSource;
        std::shared_ptr<Sandbox::Renderer>         m_renderer;
        bool                                       m_cleaned;
    };
}  // namespace Sandbox
