#pragma once
#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"

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

    class RendererSource
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
        virtual void         PushConstants(const std::shared_ptr<CommandBuffer>& inCommandBuffer);
        virtual void CustomDrawMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<DescriptorSet>& descriptorSet,
                                    uint32_t frameFlightIndex, uint32_t dynamicOffsets);
        virtual void CustomDrawOverlay(const std::shared_ptr<Sandbox::Mesh>& mesh, const std::shared_ptr<Sandbox::CommandBuffer>& shared,
                                       const std::shared_ptr<Sandbox::DescriptorSet>& descriptorSet, uint32_t frameFlightIndex, uint32_t dynamicOffsets);
        void         SyncViewAndProjection();
        void         Tick(const std::shared_ptr<Renderer>& renderer);
        void         SetCamera(const std::shared_ptr<Camera>& inCamera);
        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
        std::vector<std::shared_ptr<ShaderModule>>  shaderModules;
        std::vector<std::shared_ptr<MVPUboObjects>> uboMvp;
        std::shared_ptr<ViewAndProjection>          viewAndProjection;
        // std::shared_ptr<PipelineLayout>             pipelineLayout;
        std::shared_ptr<Pipeline>                   pipeline;
        std::shared_ptr<PipelineState>              pipelineState;
        std::shared_ptr<Camera>                     camera;

        // PushConstantsInfo pushConstantsInfo;
    };
}  // namespace Sandbox
