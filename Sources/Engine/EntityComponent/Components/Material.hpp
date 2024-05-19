#pragma once

#include "Engine/EntityComponent/IComponent.hpp"
#include "Engine/RendererSource/PbrRendererSource.hpp"
#include "Generated/Material.rfkh.h"
#include "Standard/String.hpp"
#include "VulkanRHI/Core/Device.hpp"

namespace Sandbox NAMESPACE()
{
    namespace Resource
    {
        class Image;
    }
    class RendererSource;
    class CommandBuffer;
    class Pipeline;
    class Mesh;
    class Renderer;
    class Texture;
    class Image;
    class DescriptorSet;


    class CLASS() Material : public IComponent
    {
    public:
        // std::shared_ptr<Pipeline> mainPipeline;

        // NOTE:必须要有默认参数构造
        Material();

        void Prepare(const std::shared_ptr<Renderer>& renderer);

        void                                    Cleanup() override;
        
        std::shared_ptr<Sandbox::DescriptorSet> GetDescriptorSet(uint32_t frameFlightIndex, const std::shared_ptr<RendererSource>& rendererSource);

        void DrawMesh(const std::shared_ptr<Renderer>& inRenderer, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                      const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets);
        void DrawOverlay(const std::shared_ptr<Renderer>& inRenderer, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                         const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets);

        void LoadImages(const std::shared_ptr<Renderer>& renderer);

        void UpdateDescriptorSets(const std::shared_ptr<PbrRendererSource>& rendererSource);
        // PushConstantsInfo pushConstantsInfo;

        std::array<std::shared_ptr<Texture>, 4> textures;
        std::shared_ptr<Sandbox::Image>         albedoImage;
        std::shared_ptr<Sandbox::Image>         metallicImage;
        std::shared_ptr<Sandbox::Image>         roughnessImage;
        std::shared_ptr<Sandbox::Image>         aoImage;

        std::shared_ptr<RendererSource> customRendererSource;

        FIELD()
        String albedoPath;
        FIELD()
        String metallicPath;
        FIELD()
        String roughnessPath;
        FIELD()
        String aoPath;

        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;

    private:
        uint32_t                  m_instanceId;
        bool                      m_cleaned;
        std::shared_ptr<Mesh>     m_mesh;
        std::shared_ptr<Renderer> m_renderer;
        Sandbox_Material_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Material_GENERATED
