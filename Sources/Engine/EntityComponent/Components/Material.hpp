#pragma once

#include "Engine/EntityComponent/IComponent.hpp"
#include "Generated/Material.rfkh.h"
#include "VulkanRHI/Core/PipelineLayout.hpp"

namespace Sandbox NAMESPACE()
{
    class RendererSource;
    class CommandBuffer;

    class Pipeline;
    class Mesh;

    class CLASS() Material : public IComponent
    {
    public:
        // std::shared_ptr<Pipeline> mainPipeline;
        std::shared_ptr<RendererSource> customRendererSource;

        Material();

        void DrawMesh(const std::shared_ptr<PipelineLayout>& pipelineLayout, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                      const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets);
        void DrawOverlay(const std::shared_ptr<PipelineLayout>& pipelineLayout, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                         const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets);

    private:
        std::shared_ptr<Mesh> m_mesh;

        Sandbox_Material_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Material_GENERATED
