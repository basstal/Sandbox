#pragma once
#include "PbrRendererSource.hpp"
#include "VulkanRHI/Core/Framebuffer.hpp"
#include "VulkanRHI/Rendering/RenderTarget.hpp"

namespace Sandbox
{
    /**
     * \brief 简单的顶点结构
     */
    struct SimpleVertexFlat
    {
        glm::vec2 position;
        glm::vec2 uv;
    };

    class BufferRendererSource : public PbrRendererSource // 派生自 pbr 是因为要用 pbr 的 pipeline 绘制原来的场景再把深度图单独拿出来绘制
    {
    public:
        enum BufferType
        {
            None,
            Depth,
        };
        BufferType                     bufferType = BufferType::Depth;
        std::shared_ptr<ShaderLinkage> blitDepthShaderLinkage;
        std::shared_ptr<RenderPass>    blitDepthRenderPass;

        std::shared_ptr<PipelineState> blitDepthPipelineState;
        // std::shared_ptr<Image>         blitDepthDestinationImage;

        std::shared_ptr<Sandbox::Pipeline> blitDepthPipeline;

        // std::shared_ptr<ImageView> outputImageView;

        std::vector<std::shared_ptr<RenderAttachments>> blitDepthAttachments;
        std::vector<std::shared_ptr<RenderTarget>>      blitDepthRenderTargets;
        std::vector<std::shared_ptr<DescriptorSet>>     blitDepthDescriptorSets;

        std::vector<Sandbox::SimpleVertexFlat> quadProperties;
        std::shared_ptr<Buffer>                quadBuffer;

        // std::shared_ptr<Framebuffer> blitDepthFramebuffer;

        void Prepare(std::shared_ptr<Renderer>& renderer) override;

        void Cleanup() override;

        void BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments, VkExtent2D resolution) override;

        void OnRecreateSwapchain() override;
    private:
        std::shared_ptr<Renderer> m_renderer;
    };
}  // namespace Sandbox
