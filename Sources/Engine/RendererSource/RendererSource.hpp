#pragma once
#include "Engine/EntityComponent/Components/Camera.hpp"

namespace Sandbox
{
    class Pipeline;
    class PipelineLayout;
    struct MVPUboObjects;
    class ShaderModule;
    class DescriptorSet;
    class Renderer;
    class ShaderSource;
    class RendererSource
    {
    public:
        virtual ~RendererSource() = default;

        virtual void                            Prepare(std::shared_ptr<Renderer>& renderer);
        std::shared_ptr<Sandbox::MVPUboObjects> PrepareUniformBuffers(std::shared_ptr<Renderer>& renderer);
        void                                    UpdateModels(std::shared_ptr<Renderer>& renderer, const std::vector<std::shared_ptr<Models>>& inModels);
        virtual void                            UpdateUniforms(uint32_t frameFlightIndex);
        virtual void                            Cleanup();
        virtual void                            CreatePipeline(std::shared_ptr<Renderer>& renderer)       = 0;
        virtual void                            CreateDescriptorSets(std::shared_ptr<Renderer>& renderer) = 0;

        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
        std::vector<std::shared_ptr<ShaderModule>>  shaderModules;
        std::vector<std::shared_ptr<MVPUboObjects>> uboMvp;
        std::shared_ptr<ViewAndProjection>          viewAndProjection;
        std::shared_ptr<PipelineLayout>             pipelineLayout;
        std::shared_ptr<Pipeline>                   pipeline;
    };
}  // namespace Sandbox
