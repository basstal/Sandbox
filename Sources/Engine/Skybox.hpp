#pragma once
#include <memory>
#include <vector>

#include "FileSystem/File.hpp"
#include "Images/Image.hpp"
#include "Misc/Delegate.hpp"
#include "VulkanRHI/Core/Sampler.hpp"

namespace Sandbox
{
    class Buffer;
    class Image;
    class ImageView;
    class Renderer;
    class CommandBuffer;
    class Pipeline;
    class DescriptorSet;

    class Skybox
    {
    public:
        Skybox();

        ~Skybox();

        void Prepare(const std::shared_ptr<Renderer>& renderer);
        bool IsPrepared();
        void CreateCubemapImages(const std::vector<std::shared_ptr<Resource::Image>>& skyboxImages);
        void Draw(const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t frameFlightIndex);

        void Cleanup();

    private:
        std::shared_ptr<Sandbox::Buffer> CreateVertexBuffer(const std::shared_ptr<Renderer>& renderer);

        bool m_cleaned;

        std::shared_ptr<Sandbox::Buffer> m_vertexBuffer;

        std::shared_ptr<Sandbox::Pipeline> m_pipeline;

        std::vector<std::shared_ptr<DescriptorSet>> m_descriptorSets;
        std::shared_ptr<Image>                      m_image;
        std::shared_ptr<ImageView>                  m_imageView;

        std::shared_ptr<Renderer> m_renderer;
        std::shared_ptr<Sampler>  m_sampler;

        DelegateHandle m_beforeRendererDrawHandle;
    };
}  // namespace Sandbox
