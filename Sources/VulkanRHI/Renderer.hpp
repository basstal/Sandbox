#pragma once
#include <memory>
#include <memory>
#include <queue>
#include <vector>

#include "Core/Swapchain.hpp"
#include "Misc/Event.hpp"

namespace Sandbox
{
    class Mesh;
    class RenderAttachments;
    class Window;
    struct MVPUboObjects;
    struct ViewAndProjection;
    class Fence;
    class CommandBuffer;
    class UniformBuffer;
    class Texture;
    class Semaphore;
    class Instance;
    class Surface;
    class Device;
    class CommandPool;
    class DescriptorPool;
    class Swapchain;
    class RenderPass;
    class RenderTarget;
    class ShaderModule;
    class PipelineLayout;
    class Pipeline;
    class DescriptorSet;

    struct WindowSerializedProperties;

    class Renderer
    {
    public:
        void Prepare(const std::shared_ptr<Window>& window);

        /**
         * \brief 离线绘制 3D 场景
         */
        void Draw();

        /**
         * \brief 提交到交换链
         */
        void Preset();

        void RecordCommandBuffer(std::shared_ptr<CommandBuffer>& commandBuffer);

        void UpdateUniforms(std::shared_ptr<CommandBuffer>& commandBuffer);

        void Cleanup();

        Sandbox::ESwapchainStatus AcquireNextImage();

        void OnAfterRecreateSwapchain();

        std::shared_ptr<Instance> instance;
        std::shared_ptr<WindowSerializedProperties> surfaceProperty;
        std::shared_ptr<Surface> surface;
        std::shared_ptr<Device> device;
        std::shared_ptr<CommandPool> commandPool;
        std::shared_ptr<DescriptorPool> descriptorPool;
        std::shared_ptr<Swapchain> swapchain;
        std::shared_ptr<RenderPass> renderPass;
        std::vector<std::shared_ptr<RenderTarget>> renderTargets;
        std::vector<std::shared_ptr<RenderAttachments>> renderAttachments;

        std::shared_ptr<ShaderModule> vertexShader;
        std::shared_ptr<ShaderModule> fragmentShader;
        std::shared_ptr<PipelineLayout> pipelineLayout;
        std::shared_ptr<Pipeline> pipeline;

        uint32_t maxFramesFlight = 2;
        // 根据最大飞行帧数定制大小
        std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
        std::vector<std::shared_ptr<MVPUboObjects>> uboMvp;
        // TODO:挪到其他地方去
        std::vector<std::array<std::shared_ptr<Texture>, 4>> textures;
        std::vector<std::shared_ptr<UniformBuffer>> uboLights;
        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;

        // 按需定制大小
        std::vector<std::shared_ptr<Semaphore>> imageAvailableSemaphores;
        std::vector<std::shared_ptr<Semaphore>> renderFinishedSemaphores;
        std::vector<std::shared_ptr<Fence>> fences;

        // 按绘制定制大小
        std::queue<std::shared_ptr<Mesh>> queuedMeshes;
        // std::vector<std::shared_ptr<Buffer>> vertexBuffers;
        // std::vector<std::shared_ptr<Buffer>> indexBuffers;
        // std::vector<uint32_t> indexCounts;
        /**
         * \brief 当前绘制帧序号
         */
        uint32_t frameFlightIndex = 0;

        Event<const std::shared_ptr<CommandBuffer>&, uint32_t> onBeforeRendererDraw;
        Event<const std::shared_ptr<CommandBuffer>&, uint32_t> onAfterRendererDraw;
        Event<const std::shared_ptr<CommandBuffer>&, uint32_t> onOtherDrawCommands;

        std::shared_ptr<ViewAndProjection> viewAndProjection;
        Event<uint32_t> onOtherCommandBuffer;

    private:
        bool m_cleaned = false;
    };
}
