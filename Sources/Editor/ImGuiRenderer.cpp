#include "pch.hpp"

#include "ImGuiRenderer.hpp"

#include "Editor.hpp"
#include "FileSystem/Directory.hpp"
#include "IImGuiWindow.hpp"
#include "ImGuiWidgets/MenuBar.hpp"
#include "ImGuiWindows/ContentBrowser.hpp"
#include "ImGuiWindows/DemoWindow.hpp"
#include "ImGuiWindows/Hierarchy.hpp"
#include "ImGuiWindows/Inspector.hpp"
#include "ImGuiWindows/Logs.hpp"
#include "ImGuiWindows/Stats.hpp"
#include "ImGuiWindows/Viewport.hpp"
#include "Misc/TypeCasting.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"
#include "TransformGizmo.hpp"
#include "VulkanRHI/Common/Debug.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorPool.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Fence.hpp"
#include "VulkanRHI/Core/Instance.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"
#include "VulkanRHI/Rendering/RenderTarget.hpp"

void Sandbox::ImGuiRenderer::Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor)
{
    m_renderer = renderer;
    // 构建 GUI renderPass
    std::vector<Attachment> attachments = {
        {VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
    };
    std::vector<LoadStoreInfo> loadStoreInfos = {
        {VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE},
    };
    std::vector<SubpassInfo> subpassInfos = {
        {{0}, {}, true},
    };
    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass    = 0;
    subpassDependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    renderPass                      = std::make_shared<RenderPass>(renderer->device, attachments, loadStoreInfos, subpassInfos, subpassDependency);
    ImGuiPrepare(renderer, renderPass);

    auto maxFramesFlight = m_renderer->maxFramesFlight;
    commandBuffers.resize(maxFramesFlight);
    fences.resize(maxFramesFlight);
    for (size_t i = 0; i < commandBuffers.size(); ++i)
    {
        commandBuffers[i] = std::make_shared<CommandBuffer>(m_renderer->device, m_renderer->commandPool);
        fences[i]         = std::make_shared<Fence>(m_renderer->device);
    }

    m_editor = editor;
    menuBar  = std::make_shared<MenuBar>();
    renderer->swapchain->onAfterRecreateSwapchain.BindMember<ImGuiRenderer, &ImGuiRenderer::CreateRenderTarget>(this);
    CreateRenderTarget();
    RegisterWindows(renderer);
    m_prepared = true;
}

void Sandbox::ImGuiRenderer::RegisterWindows(const std::shared_ptr<Renderer>& renderer)
{
    RegisterWindow(std::make_shared<DemoWindow>());  // TODO: for debug, remove in future
    viewport = std::make_shared<Viewport>(renderer);
    viewport->Prepare();
    RegisterWindow(viewport);
    RegisterWindow(std::make_shared<ContentBrowser>());
    auto inspector = std::make_shared<Inspector>();
    hierarchy      = std::make_shared<Hierarchy>(inspector);
    hierarchy->onTargetChanged.BindMember<Inspector, &Inspector::InspectTarget>(inspector.get());
    hierarchy->onTargetChanged.BindMember<Viewport, &Viewport::InspectTarget>(viewport.get());
    RegisterWindow(hierarchy);
    RegisterWindow(inspector);
    RegisterWindow(std::make_shared<Logs>());
    RegisterWindow(std::make_shared<Stats>());
}

void Sandbox::ImGuiRenderer::UnregisterAllWindows()
{
    for (auto& window : windows)
    {
        window->Cleanup();
    }
    windows.clear();
}

void Sandbox::ImGuiRenderer::ImGuiPrepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<RenderPass>& inRenderPass)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    float fontSize = 22.f;
    auto  firaCode = Directory::GetAssetsDirectory().GetFile("Fonts/FiraCode-Regular.ttf");
    // 加载主编程字体 - Fira Code
    // ImFont* firaCodeFont =
    io.Fonts->AddFontFromFileTTF(firaCode.path.generic_string().c_str(), fontSize);

    // 加载中文字符支持的字体 - 微软雅黑
    // 注意: 这里的大小和主字体大小相匹配是很重要的，以保证文本渲染时的一致性
    static const ImWchar icons_ranges[] = {0x0020, 0xFFFF, 0};
    ImFontConfig         icons_config;
    icons_config.MergeMode  = true;
    icons_config.PixelSnapH = true;
    auto msyh               = Directory::GetAssetsDirectory().GetFile("Fonts/msyh.ttc");
    io.Fonts->AddFontFromFileTTF(msyh.path.generic_string().c_str(), fontSize, &icons_config, icons_ranges);
    io.Fonts->Build();

    // // 可以添加其他字体作为备选，例如 Courier New
    // io.Fonts->AddFontFromFileTTF("path_to_courier_new.ttf", 18.0f, &icons_config, icons_ranges);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(renderer->surface->window->glfwWindow, false);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                  = renderer->instance->vkInstance;
    init_info.PhysicalDevice            = renderer->device->vkPhysicalDevice;
    init_info.Device                    = renderer->device->vkDevice;
    init_info.QueueFamily               = *renderer->device->queueFamilyIndices.graphicsFamily;
    init_info.Queue                     = renderer->device->graphicsQueue;
    init_info.PipelineCache             = VK_NULL_HANDLE;  // TODO: PipelineCache
    init_info.DescriptorPool            = renderer->descriptorPool->vkDescriptorPool;
    init_info.RenderPass                = inRenderPass->vkRenderPass;
    init_info.Subpass                   = 0;
    init_info.MinImageCount             = static_cast<uint32_t>(renderer->swapchain->vkImages.size());
    init_info.ImageCount                = static_cast<uint32_t>(renderer->swapchain->vkImages.size());
    init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator                 = nullptr;
    init_info.CheckVkResultFn           = ValidateVkResult;
    ImGui_ImplVulkan_Init(&init_info);
}

void Sandbox::ImGuiRenderer::Cleanup()
{
    if (!m_prepared)
    {
        return;
    }
    m_prepared = false;
    UnregisterAllWindows();
    for (auto& commandBuffer : commandBuffers)
    {
        commandBuffer->Cleanup();
    }
    for (auto& fence : fences)
    {
        fence->Cleanup();
    }
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    for (auto& renderTarget : renderTargets)
    {
        renderTarget->Cleanup();
    }
    for (auto& renderAttachment : renderAttachments)
    {
        renderAttachment->Cleanup();
    }
    renderPass->Cleanup();
}

void Sandbox::ImGuiRenderer::CreateRenderTarget()
{
    auto imageViewSize = m_renderer->swapchain->imageViews.size();
    ImGui_ImplVulkan_SetMinImageCount(ToUInt32(imageViewSize));

    renderAttachments.resize(imageViewSize);
    renderTargets.resize(imageViewSize);
    for (auto& renderTarget : renderTargets)
    {
        renderTarget != nullptr ? renderTarget->Cleanup() : void();
    }
    for (auto& renderAttachment : renderAttachments)
    {
        renderAttachment != nullptr ? renderAttachment->Cleanup() : void();
    }
    for (size_t i = 0; i < imageViewSize; ++i)
    {
        renderAttachments[i] = std::make_shared<RenderAttachments>(m_renderer->device, renderPass, m_renderer->swapchain->imageExtent, m_renderer->swapchain->imageViews[i]);
        renderTargets[i]     = std::make_shared<RenderTarget>(m_renderer->device, renderPass, m_renderer->swapchain->imageExtent, renderAttachments[i]);
    }
}


void Sandbox::ImGuiRenderer::DrawBackgroundDockingArea(int windowX, int windowY, int windowWidth, int windowHeight)
{
    // 获取菜单栏的高度
    float menuBarHeight = ImGui::GetFrameHeight();
    // 构造一个不可移动的隐藏窗口用于 docking，设置下一个窗口的位置和大小，以排除菜单栏的高度
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowX), static_cast<float>(windowY) + menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight) - menuBarHeight));
    ImGui::Begin("BackgroundDockingArea", nullptr, ImGuiWindowFlags_NoDecoration);
    ImGui::End();
}

void Sandbox::ImGuiRenderer::Draw()
{
    // 初始化 ImGui 帧信息
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto glfwWindow = m_renderer->surface->window->glfwWindow;
    int  windowX, windowY, windowWidth, windowHeight;
    glfwGetWindowPos(glfwWindow, &windowX, &windowY);
    glfwGetWindowSize(glfwWindow, &windowWidth, &windowHeight);
    menuBar->Draw(windows);
    DrawBackgroundDockingArea(windowX, windowY, windowWidth, windowHeight);

    // 绘制其他窗口
    for (auto& window : windows)
    {
        window->OnGuiBegin();
    }

    // 结束 ImGui 帧信息
    ImGui::Render();

    auto frameFlightIndex = m_renderer->frameFlightIndex;
    // 提交到 vulkan 绘制
    auto& commandBuffer = commandBuffers[frameFlightIndex];
    commandBuffer->Reset();
    RecordCommandBuffer(commandBuffer);
    commandBuffer->Submit(fences[frameFlightIndex], {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {m_renderer->imageAvailableSemaphores[frameFlightIndex]},
                          {m_renderer->renderFinishedSemaphores[frameFlightIndex]});
    fences[frameFlightIndex]->WaitForFence();

    ImGuiIO& io = ImGui::GetIO();
    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        // 将其他 viewport 的 glfw 事件也加入到 callbackBridge 中一起广播
        auto platformIO = ImGui::GetPlatformIO();
        for (const ImGuiViewport* platformViewport : platformIO.Viewports)
        {
            GLFWwindow* windowPtr = reinterpret_cast<GLFWwindow*>(platformViewport->PlatformHandle);
            m_editor->window->callbackBridge->BindCallbacks(windowPtr);
        }
    }
}

void Sandbox::ImGuiRenderer::RecordCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer)
{
    commandBuffer->Begin();
    {
        VkClearColorValue        clearColor        = {{0.8f, 0.8f, 0.8f, 1.0f}};
        VkClearDepthStencilValue clearDepthStencil = {1.0f, 0};
        // TODO:只用一个 framebuffer？
        auto  imageIndex  = m_renderer->swapchain->acquiredNextImageIndex;
        auto& framebuffer = renderTargets[imageIndex]->framebuffer;
        commandBuffer->BeginRenderPass(renderPass, framebuffer, m_renderer->swapchain->imageExtent, clearColor, clearDepthStencil);
        // Record dear imGui primitives into command buffer
        ImDrawData* mainDrawData = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(mainDrawData, commandBuffer->vkCommandBuffer);
        // Submit command buffer
        commandBuffer->EndRenderPass();
    }
    commandBuffer->End();
}

void Sandbox::ImGuiRenderer::RegisterWindow(const std::shared_ptr<IImGuiWindow>& inWindow)
{
    for (auto& window : windows)
    {
        if (window == inWindow)
        {
            return;
        }
    }
    inWindow->Prepare();
    windows.push_back(inWindow);
}

void Sandbox::ImGuiRenderer::Tick(float deltaTime)
{
    for (auto& window : windows)
    {
        window->Tick(deltaTime);
    }
}
