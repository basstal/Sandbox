#include "pch.hpp"

#include "Viewport.hpp"

#include "ComponentInspectors/TransformInspector.hpp"
#include "Engine/Camera.hpp"
#include "Engine/EntityComponent/Components/Transform.hpp"
#include "Engine/EntityComponent/GameObject.hpp"
#include "Misc/Debug.hpp"
#include "Misc/GlmExtensions.hpp"
#include "Misc/TypeCasting.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"
#include "VulkanRHI/Common/ViewMode.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Sampler.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"

void Sandbox::Viewport::OnCursorPosition(GLFWwindow* window, double xPos, double yPos)
{
    if (m_mouseMoved)
    {
        m_lastX      = static_cast<float>(xPos);
        m_lastY      = static_cast<float>(yPos);
        m_mouseMoved = false;
    }

    float offsetX = static_cast<float>(xPos) - m_lastX;
    float offsetY = m_lastY - static_cast<float>(yPos);
    m_lastX       = static_cast<float>(xPos);
    m_lastY       = static_cast<float>(yPos);
    mainCamera->ProcessMouseMovement(offsetX, offsetY);
}

Sandbox::Viewport::Viewport(const std::shared_ptr<Renderer>& inRenderer)
{
    LOGD("Construct Viewport\n{}", GetCallStack())
    name           = "Viewport";
    flags          = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    m_renderer     = inRenderer;
    presentSampler = std::make_shared<Sampler>(m_renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    m_renderer->swapchain->onAfterRecreateSwapchain.BindMember<Viewport, &Viewport::OnRecreateFramebuffer>(this);
    m_renderer->surface->window->callbackBridge->onMouseButton.BindMember<Viewport, &Viewport::SwitchCursorForCameraMovement>(this);
}

void Sandbox::Viewport::Prepare()
{
    IImGuiWindow::Prepare();
    OnRecreateFramebuffer();
}

void Sandbox::Viewport::OnGui()
{
    m_renderer->Draw();
    // ImVec2 windowPos = ImGui::GetWindowPos();
    // ImVec2 windowSize = ImGui::GetWindowSize();
    auto imageIndex     = m_renderer->swapchain->acquiredNextImageIndex;
    m_imguiWindow       = ImGui::GetCurrentWindow();
    auto viewportWidth  = m_imguiWindow->InnerRect.GetWidth();
    auto viewportHeight = m_imguiWindow->InnerRect.GetHeight();
    auto resolution     = m_renderer->resolution;
    m_startPosition     = CalculateStartPosition(16, 9, ToInt32(viewportWidth), ToInt32(viewportHeight), resolution.width, resolution.height);
    ImGui::SetCursorPos(m_startPosition);
    ImGui::Image((ImTextureID)presentDescriptorSets[imageIndex], ImVec2(ToFloat(resolution.width), ToFloat(resolution.height)));
    m_glfwWindow               = reinterpret_cast<GLFWwindow*>(m_imguiWindow->Viewport->PlatformHandle);
    m_isMouseHoveringInnerRect = ImGui::IsMouseHoveringRect(m_imguiWindow->InnerRect.Min, m_imguiWindow->InnerRect.Max);
    flags                      = ImGui::IsWindowHovered() && m_isMouseHoveringInnerRect ? flags | ImGuiWindowFlags_NoMove : flags ^ ImGuiWindowFlags_NoMove;
    if ((flags & ImGuiWindowFlags_NoMove) != 0)
    {
        ImGui::SetNextFrameWantCaptureMouse(true);
    }
    // auto ray = std::make_shared<Ray>(CursorPositionToWorldRay(m_renderer->surface->window->glfwWindow, m_editor->camera->GetViewMatrix(),
    //                                                           m_editor->camera->GetProjectionMatrix()));
    // LOGD(ray->ToString())
    // if (m_editor->transformGizmo->cameraRay)
    // {
    //     m_editor->transformGizmo->cameraRay->origin = m_editor->camera->position;
    //     LOGD("camera front : {}", Sandbox::ToString(m_editor->camera->front))
    //     m_editor->transformGizmo->cameraRay->direction = ray->direction;
    // }
    // else
    // {
    //     m_editor->transformGizmo->cameraRay = ray;
    // }
    // m_editor->transformGizmo->cameraRay->PrepareDebugDrawData(m_renderer->device);
    DrawGizmo(resolution);
}

void Sandbox::Viewport::Tick(float deltaTime)
{
    IImGuiWindow::Tick(deltaTime);
    if (m_isCameraMovementEnabled && m_glfwWindow)
    {
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        {
            mainCamera->ProcessKeyboard(ECameraMovement::FORWARD, deltaTime);
        }
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        {
            mainCamera->ProcessKeyboard(ECameraMovement::BACKWARD, deltaTime);
        }
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        {
            mainCamera->ProcessKeyboard(ECameraMovement::LEFT, deltaTime);
        }
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        {
            mainCamera->ProcessKeyboard(ECameraMovement::RIGHT, deltaTime);
        }
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            mainCamera->ProcessKeyboard(ECameraMovement::UP, deltaTime);
        }
    }
}

void Sandbox::Viewport::SwitchCursorForCameraMovement(GLFWwindow* inWindow, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_RIGHT)
    {
        return;
    }
    auto shouldEnabled = action == 1;
    if (inWindow != m_glfwWindow || !m_isMouseHoveringInnerRect)
    {
        DisableCameraMovement();
        return;
    }
    if (m_isCameraMovementEnabled == shouldEnabled)
    {
        return;
    }
    auto window = m_renderer->surface->window;
    if (!m_isCameraMovementEnabled)  // 如果之前没有开启
    {
        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_cursorBindHandler       = window->callbackBridge->onCursorPosition.BindMember<Viewport, &Viewport::OnCursorPosition>(this);
        m_isCameraMovementEnabled = true;
    }
    else  // 如果之前是开启的，现在要关闭
    {
        DisableCameraMovement();
    }
}

void Sandbox::Viewport::DisableCameraMovement()
{
    auto window = m_renderer->surface->window;
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    window->callbackBridge->onCursorPosition.Unbind(m_cursorBindHandler);
    m_cursorBindHandler       = Sandbox::DelegateHandle::Null;
    m_mouseMoved              = true;
    m_isCameraMovementEnabled = false;
}

void Sandbox::Viewport::Cleanup()
{
    IImGuiWindow::Cleanup();
    presentSampler->Cleanup();

    for (auto& presentDescriptorSet : presentDescriptorSets)
    {
        ImGui_ImplVulkan_RemoveTexture(presentDescriptorSet);
    }
}

void Sandbox::Viewport::SetTarget(const std::shared_ptr<GameObject>& target) { m_referenceGameObject = target; }

void Sandbox::Viewport::DrawGizmo(VkExtent2D extent2D)
{
    if (m_imguiWindow == nullptr)
    {
        return;
    }

    if (transformInspector != nullptr && m_referenceGameObject != nullptr)
    {
        glm::mat4    model           = m_referenceGameObject->transform->GetModelMatrix();
        glm::mat4    view            = mainCamera->GetViewMatrix();
        glm::mat4    projection      = mainCamera->GetProjectionMatrix();
        float const* cameraViewConst = glm::value_ptr(view);
        float        cameraView[16];
        std::copy(cameraViewConst, cameraViewConst + 16, cameraView);
        float const* cameraProjection = glm::value_ptr(projection);
        auto         matrixConst      = glm::value_ptr(model);
        float        matrix[16];
        std::copy(matrixConst, matrixConst + 16, matrix);

        // static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

        // static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
        // static bool useSnap = false;
        // static float snap[3] = {1.f, 1.f, 1.f};
        static float bounds[]        = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
        static float boundsSnap[]    = {0.1f, 0.1f, 0.1f};
        static bool  boundSizing     = false;
        static bool  boundSizingSnap = false;
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        auto rectPosition = m_imguiWindow->InnerRect.Min;
        // TODO: 双显示器的情况下， rectPosition.x 为负数会导致 gizmo 无法显示
        ImGuizmo::SetRect(rectPosition.x + m_startPosition.x, rectPosition.y + m_startPosition.y, ToFloat(extent2D.width), ToFloat(extent2D.height));
        ImGuizmo::Manipulate(cameraView, cameraProjection, transformInspector->currentGizmoOperation, transformInspector->currentGizmoMode, matrix, nullptr,
                             transformInspector->useSnap ? &transformInspector->snap[0] : nullptr, boundSizing ? bounds : nullptr, boundSizingSnap ? boundsSnap : nullptr);

        m_referenceGameObject->transform->position = glm::vec3(matrix[12], matrix[13], matrix[14]);
        // static float camDistance = 8.f;
        // ImGuiIO& io = ImGui::GetIO();
        // float viewManipulateRight = io.DisplaySize.x;
        // float viewManipulateTop = 0;
        // ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

        // EditTransform(cameraView, cameraProjection, matrix, true);
    }

    // static int location = 0;
    // ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    // ImVec2 childPos, childPosPivot;
    const float padding = 10.0f;
    // if (location >= 0)
    // {
    // 	// ImVec2 workPos = m_imguiWindow->InnerRect.GetTL();
    // 	// ImVec2 workSize = m_imguiWindow->InnerRect.GetSize();
    // 	// childPos.x = (location & 1) ? (workPos.x + workSize.x - pad) : (workPos.x + pad);
    // 	// childPos.y = (location & 2) ? (workPos.y + workSize.y - pad) : (workPos.y + pad);
    // 	// childPosPivot.x = (location & 1) ? 1.0f : 0.0f;
    // 	// childPosPivot.y = (location & 2) ? 1.0f : 0.0f;
    // }
    // else if (location == -2)
    // {
    // 	// Center window
    // 	// childPos = m_imguiWindow->InnerRect.GetCenter();
    // 	// childPosPivot = ImVec2(0.5f, 0.5f);
    // }
    // 先保存当前的背景颜色
    ImVec4 prevChildBg = ImGui::GetStyle().Colors[ImGuiCol_ChildBg];

    // 设置新的背景颜色和透明度
    ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = ImVec4(prevChildBg.x, prevChildBg.y, prevChildBg.z, 0.35f);  // 设置为半透明
    // 设置子窗口的起始位置
    ImGui::SetCursorPos(ImVec2(padding, padding));
    auto childFlags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY;
    if (ImGui::BeginChild("Example: Simple overlay", ImVec2(0, 0), childFlags, windowFlags))
    {
        auto viewModeName = VIEW_MODE_NAMES[m_renderer->viewMode];
        // Simple selection popup (if you want to show the current selection inside the Button itself,
        // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        ImVec2 textSize = ImGui::CalcTextSize(viewModeName);
        if (ImGui::Button(viewModeName, ImVec2(textSize.x + padding * 2, textSize.y + padding)))
            ImGui::OpenPopup("ViewModePopup");
        if (ImGui::BeginPopup("ViewModePopup"))
        {
            // ImGui::SeparatorText("Aquarium");
            for (int i = 0; i < IM_ARRAYSIZE(VIEW_MODE_NAMES); i++)
            {
                if (ImGui::Selectable(VIEW_MODE_NAMES[i]))
                {
                    m_renderer->onViewModeChanged.Trigger(static_cast<EViewMode>(i));
                }
            }
            ImGui::EndPopup();
        }
        // IMGUI_DEMO_MARKER("Examples/Simple Overlay");
        // ImGui::Text("Simple overlay\n" "(right-click to change position)");
        // ImGui::Separator();
        // if (ImGui::IsMousePosValid())
        // 	ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        // else
        // 	ImGui::Text("Mouse Position: <invalid>");
        // if (ImGui::BeginPopupContextWindow())
        // {
        // 	if (ImGui::MenuItem("Custom", NULL, location == -1))
        // 		location = -1;
        // 	if (ImGui::MenuItem("Center", NULL, location == -2))
        // 		location = -2;
        // 	if (ImGui::MenuItem("Top-left", NULL, location == 0))
        // 		location = 0;
        // 	if (ImGui::MenuItem("Top-right", NULL, location == 1))
        // 		location = 1;
        // 	if (ImGui::MenuItem("Bottom-left", NULL, location == 2))
        // 		location = 2;
        // 	if (ImGui::MenuItem("Bottom-right", NULL, location == 3))
        // 		location = 3;
        // 	if (m_open && ImGui::MenuItem("Close"))
        // 		m_open = false;
        // 	ImGui::EndPopup();
        // }
    }
    ImGui::EndChild();
    // 恢复之前的背景颜色
    ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = prevChildBg;
}

void Sandbox::Viewport::OnRecreateFramebuffer()
{
    auto imageViewSize = m_renderer->swapchain->imageViews.size();
    presentDescriptorSets.resize(imageViewSize);
    for (size_t i = 0; i < imageViewSize; ++i)
    {
        VkImageView offlineImageView = m_renderer->renderAttachments[i]->resolveImageView->vkImageView;  // Vulkan图像视图，已经创建并指向包含渲染结果的图像
        presentDescriptorSets[i]     = ImGui_ImplVulkan_AddTexture(presentSampler->vkSampler, offlineImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}
void Sandbox::Viewport::InspectTarget(std::shared_ptr<GameObject> inTarget)
{
    transformInspector = inTarget == nullptr ? nullptr : std::dynamic_pointer_cast<TransformInspector>(Inspector::ComponentMapping["Transform"]);
}

/**
 * @brief 计算符合宽高比的起始位置
 *
 * @param aspectWidth
 * @param aspectHeight
 * @param viewportWidth
 * @param viewportHeight
 */
ImVec2 Sandbox::Viewport::CalculateStartPosition(int aspectWidth, int aspectHeight, int viewportWidth, int viewportHeight, uint32_t& adjustedWidth, uint32_t& adjustedHeight)
{
    // 计算目标分辨率的宽高比
    float targetAspectRatio = ToFloat(aspectWidth) / ToFloat(aspectHeight);

    // 计算实际分辨率的宽高比
    float actualAspectRatio = static_cast<float>(viewportWidth) / ToFloat(viewportHeight);

    // 初始化起始位置
    int startX = 0, startY = 0;

    if (actualAspectRatio > targetAspectRatio)
    {
        // // 如果实际宽高比大于目标宽高比，则按照高度进行缩放，并计算左右的黑边
        // float scaledWidth = targetAspectRatio * viewportHeight;
        // startX = static_cast<int>((viewportWidth - scaledWidth) / 2);

        float scaleFactor = static_cast<float>(viewportHeight) / ToFloat(adjustedHeight);
        adjustedHeight    = viewportHeight;
        adjustedWidth     = ToUInt32(ToFloat(adjustedWidth) * scaleFactor);
        // 重新计算起始X坐标
        startX = (viewportWidth - ToInt32(adjustedWidth)) / 2;
    }
    else if (actualAspectRatio < targetAspectRatio)
    {
        // // 如果实际宽高比小于目标宽高比，则按照宽度进行缩放，并计算上下的黑边
        // float scaledHeight = viewportWidth / targetAspectRatio;
        // startY = static_cast<int>((viewportHeight - scaledHeight) / 2);

		float scaleFactor = static_cast<float>(viewportWidth) / ToFloat(adjustedWidth);
        adjustedWidth = viewportWidth;
        adjustedHeight = ToUInt32(ToFloat(adjustedHeight) * scaleFactor);
        // 重新计算起始Y坐标
        startY = (viewportHeight - ToInt32(adjustedHeight)) / 2;
    }
    // 如果实际宽高比等于目标宽高比，起始位置保持为0，0，不需要计算

    return ImVec2(ToFloat(startX), ToFloat(startY));
}
