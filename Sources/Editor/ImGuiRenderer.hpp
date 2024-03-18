#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

#include "Misc/Event.hpp"

namespace Sandbox
{
    class Fence;
    class IImGuiWindow;
    class CommandBuffer;
    class Renderer;
    class RenderPass;
    class Editor;
    class RenderTarget;
    class RenderAttachments;
    class Sampler;
    class Viewport;

    class ImGuiRenderer
    {
    public:
        void Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor);

        void RegisterWindows(const std::shared_ptr<Renderer>& renderer);

        void UnregisterAllWindows();

        void ImGuiPrepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<RenderPass>& inRenderPass);

        void Cleanup();

        void CreateRenderTarget();

        void DrawMenuBar();

        void DrawBackgroundDockingArea(int windowX, int windowY, int windowWidth, int windowHeight);

        void Draw();

        void RecordCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer);

        void RegisterWindow(const std::shared_ptr<IImGuiWindow>& inWindow);

        void UnregisterWindow(const std::shared_ptr<IImGuiWindow>& inWindow);

        void Tick(float deltaTime);

        std::shared_ptr<RenderPass> renderPass;
        // //TODO:Debug usage
        // bool showImGuiDemoWindow = true;
        std::vector<std::shared_ptr<RenderTarget>> renderTargets;
        std::vector<std::shared_ptr<RenderAttachments>> renderAttachments;

        // Event<void> onOtherImGuiWindow;

        std::vector<std::shared_ptr<IImGuiWindow>> windows;
        std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
        std::vector<std::shared_ptr<Fence>> fences;
        std::shared_ptr<Viewport> viewport;

    private:
        std::shared_ptr<Editor> m_editor;
        std::shared_ptr<Renderer> m_renderer;
        bool m_prepared = false;
    };
}
