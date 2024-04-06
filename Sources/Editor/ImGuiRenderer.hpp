#pragma once
#include <memory>


namespace Sandbox
{
    namespace Resource
    {
        class Image;
    }
    class MenuBar;
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
    class Hierarchy;
    class Image;
    class ImageView;

    class ImGuiRenderer
    {
    public:
        void Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor);
        void LoadImageToGuiTexture(const std::string& name, const std::string& assetPath);

        void RegisterWindows(const std::shared_ptr<Renderer>& renderer);

        void UnregisterAllWindows();

        void ImGuiPrepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<RenderPass>& inRenderPass);

        void Cleanup();

        void CreateRenderTarget();

        void DrawBackgroundDockingArea(int windowX, int windowY, int windowWidth, int windowHeight);

        void Draw();

        void RecordCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer);

        void RegisterWindow(const std::shared_ptr<IImGuiWindow>& inWindow);

        void Tick(float deltaTime);

        std::shared_ptr<RenderPass> renderPass;
        // //TODO:Debug usage
        // bool showImGuiDemoWindow = true;
        std::vector<std::shared_ptr<RenderTarget>>      renderTargets;
        std::vector<std::shared_ptr<RenderAttachments>> renderAttachments;

        // Event<void> onOtherImGuiWindow;

        std::vector<std::shared_ptr<IImGuiWindow>>  windows;
        std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
        std::vector<std::shared_ptr<Fence>>         fences;
        std::shared_ptr<Viewport>                   viewport;
        std::shared_ptr<Hierarchy>                  hierarchy;
        std::shared_ptr<MenuBar>                    menuBar;

        static std::map<std::string, std::shared_ptr<Resource::Image>> guiNameToResourceImage; // TODO:先存着后面再看如何简化
        static std::map<std::string, std::shared_ptr<Image>> guiNameToImage; // TODO: 先存着后面再看如何简化
        static std::map<std::string, std::shared_ptr<ImageView>> guiNameToImageView; // TODO: 先存着后面再看如何简化
        static std::map<std::string, VkDescriptorSet> guiNameToTextureId; // ImGui_ImplVulkan_AddTexture Register a texture (VkDescriptorSet == ImTextureID)

    private:
        std::shared_ptr<Editor>   m_editor;
        std::shared_ptr<Renderer> m_renderer;
        bool                      m_prepared = false;
    };
}  // namespace Sandbox
