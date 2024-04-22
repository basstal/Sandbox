﻿#pragma once
#include <memory>
#include <queue>
#include <vector>

#include "Common/ViewMode.hpp"
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
    class RendererSource;
    class Material;
    class PipelineCaching;
    class ShaderModuleCaching;
    class DescriptorSetCaching;

    struct WindowSerializedProperties;

    class Renderer : public std::enable_shared_from_this<Renderer>
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
        void FrameFlightIndexIncrease();

        void RecordCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RendererSource>&rendererSource);

        void UpdateUniforms(std::shared_ptr<CommandBuffer>& commandBuffer, std::shared_ptr<RendererSource>& rendererSource);

        void Cleanup();

        Sandbox::ESwapchainStatus                AcquireNextImage();
        std::shared_ptr<Sandbox::RendererSource> GetCurrentRendererSource();

        void OnAfterRecreateSwapchain();

        uint32_t GetUniformDynamicAlignment(VkDeviceSize dynamicAlignment) const;
        bool     TryGetRendererSource(EViewMode viewMode, std::shared_ptr<RendererSource>& outRendererSource);

        std::shared_ptr<Instance>                       instance;
        std::shared_ptr<WindowSerializedProperties>     surfaceProperty;
        std::shared_ptr<Surface>                        surface;
        std::shared_ptr<Device>                         device;
        std::shared_ptr<CommandPool>                    commandPool;
        std::shared_ptr<DescriptorPool>                 descriptorPool;
        std::shared_ptr<Swapchain>                      swapchain;
        std::shared_ptr<RenderPass>                     renderPass;
        std::vector<std::shared_ptr<RenderTarget>>      renderTargets;
        std::vector<std::shared_ptr<RenderAttachments>> renderAttachments;

        std::shared_ptr<PipelineCaching>      pipelineCaching;
        std::shared_ptr<ShaderModuleCaching>  shaderModuleCaching;
        std::shared_ptr<DescriptorSetCaching> descriptorSetCaching;
        // std::shared_ptr<Pipeline>             pipeline;

        uint32_t maxFramesFlight = 2;
        // 根据最大飞行帧数定制大小
        std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;

        std::map<EViewMode, std::shared_ptr<RendererSource>> rendererSourceMapping;

        // 按需定制大小
        std::vector<std::shared_ptr<Semaphore>> imageAvailableSemaphores;
        std::vector<std::shared_ptr<Semaphore>> renderFinishedSemaphores;
        std::vector<std::shared_ptr<Fence>>     fences;

        // 按绘制定制大小
        std::vector<std::shared_ptr<Material>> queuedMaterials;
        /**
         * \brief 当前绘制帧序号
         */
        uint32_t frameFlightIndex = 0;

        Event<const std::shared_ptr<CommandBuffer>&, uint32_t>                         onBeforeRendererDraw;
        Event<const std::shared_ptr<CommandBuffer>&, uint32_t>                         onAfterRendererDraw;
        Event<const std::shared_ptr<CommandBuffer>&, uint32_t>                         onOtherDrawCommands;
        Event<const std::shared_ptr<CommandBuffer>&, uint32_t, std::shared_ptr<Mesh>&> onBeforeDrawMesh;
        Event<const std::shared_ptr<CommandBuffer>&, uint32_t, std::shared_ptr<Mesh>&> onAfterDrawMesh;

        Event<uint32_t> onOtherCommandBuffer;

        VkExtent2D       resolution;
        EViewMode        viewMode;
        Event<EViewMode> onViewModeChanged;

    private:
        void OnViewModeChanged(EViewMode viewMode);

	    bool m_cleaned = false;
	};
} // namespace Sandbox
