#pragma once
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"

namespace Sandbox
{
    class Pipeline;
    class PipelineLayout;
    struct MVPUboObjects;
    class ShaderModule;
    class DescriptorSet;
    class Renderer;
    class ShaderSource;
    class PipelineState;
    class ShaderLinkage;
    class Camera;
    struct Models;
    struct ViewAndProjection;
    class CommandBuffer;
    class Mesh;

    class RendererSource : public std::enable_shared_from_this<RendererSource>
    {
    public:
        virtual ~RendererSource() = default;

        virtual void                            Prepare(std::shared_ptr<Renderer>& renderer);
        std::shared_ptr<Sandbox::MVPUboObjects> PrepareUniformBuffers(std::shared_ptr<Renderer>& renderer);
        void                                    RecreateUniformModels(const std::shared_ptr<Renderer>& renderer);

        void UpdateModels(const std::shared_ptr<Renderer>& renderer, const std::vector<std::shared_ptr<Models>>& inModels, const std::vector<std::shared_ptr<Mesh>>& inMeshes);
        virtual void UpdateUniforms(uint32_t frameFlightIndex);
        virtual void Cleanup();
        virtual void CreatePipeline(std::shared_ptr<Renderer>& renderer)             = 0;
        virtual void CreateDescriptorSets(std::shared_ptr<Renderer>& renderer)       = 0;
        virtual void UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer) = 0;
        virtual void PushConstants(const std::shared_ptr<CommandBuffer>& inCommandBuffer);
        virtual void BindPipeline(const std::shared_ptr<CommandBuffer>& inCommandBuffer);
        virtual void CustomDrawMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<DescriptorSet>& descriptorSet,
                                    uint32_t frameFlightIndex, uint32_t dynamicOffsets);
        virtual void CustomDrawOverlay(const std::shared_ptr<Sandbox::Mesh>& mesh, const std::shared_ptr<Sandbox::CommandBuffer>& shared,
                                       const std::shared_ptr<Sandbox::DescriptorSet>& descriptorSet, uint32_t frameFlightIndex, uint32_t dynamicOffsets);
        void         SyncViewAndProjection();
        virtual void Tick(const std::shared_ptr<Renderer>& renderer);
        void         SetCamera(const std::shared_ptr<Camera>& inCamera);
        virtual void BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments, VkExtent2D resolution);

        virtual void OnRecreateSwapchain();

        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
        std::shared_ptr<ShaderLinkage>              shaderLinkage;
        std::vector<std::shared_ptr<MVPUboObjects>> uboMvp;
        std::vector<std::shared_ptr<MVPUboObjects>> uboMvpNoMoving;
        std::shared_ptr<ViewAndProjection>          viewAndProjection;
        std::shared_ptr<ViewAndProjection>          viewAndProjectionNoMoving;
        // std::shared_ptr<PipelineLayout>             pipelineLayout;
        // std::shared_ptr<Pipeline>                   pipeline;
        std::shared_ptr<PipelineState> pipelineState;
        std::shared_ptr<Camera>        camera;
        std::shared_ptr<Image>         outputImage;
        std::shared_ptr<ImageView>     outputImageView;


        // PushConstantsInfo pushConstantsInfo;
    };
}  // namespace Sandbox
