#include "pch.hpp"

#include "Viewport.hpp"

#include "Editor/ImGuiRenderer.hpp"
#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Components/Transform.hpp"
#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "FileSystem/Directory.hpp"
#include "Generated/Viewport.rfks.h"
#include "Inspector.hpp"
#include "Misc/Debug.hpp"
#include "Misc/GlmExtensions.hpp"
#include "Misc/TypeCasting.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"
#include "VulkanRHI/Common/ViewMode.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/Sampler.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"


Sandbox::Viewport::Viewport(const std::shared_ptr<Renderer>& inRenderer)
{
    LOGD_OLD("Construct Viewport\n{}", GetCallStack())
    // LoadFromFile()
    name           = "Viewport";
    flags          = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    m_renderer     = inRenderer;
    presentSampler = std::make_shared<Sampler>(m_renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    m_renderer->swapchain->onAfterRecreateSwapchain.BindMember<Viewport, &Viewport::OnRecreateFramebuffer>(this);
    m_renderer->surface->window->callbackBridge->onMouseButton.BindMember<Viewport, &Viewport::SwitchCursorForCameraMovement>(this);
    BindCameraPosition(inRenderer->viewMode);
    inRenderer->onViewModeChanged.BindMember<Viewport, &Viewport::BindCameraPosition>(this);
    IComponent::onComponentCreate.Bind(
        [this](const std::shared_ptr<IComponent>& inComponent)
        {
            if (mainCamera == nullptr && inComponent->GetDerivedClass() == &Camera::staticGetArchetype())
            {
                mainCamera = std::dynamic_pointer_cast<Camera>(inComponent);
            }
        });
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
    if (mainCamera != nullptr)
    {
        if (m_isCameraMovementEnabled && m_glfwWindow)
        {
            if (glfwGetKey(m_glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
            {
                mainCamera->ProcessKeyboard(Camera::ECameraMovement::FORWARD, deltaTime);
            }
            if (glfwGetKey(m_glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
            {
                mainCamera->ProcessKeyboard(Camera::ECameraMovement::BACKWARD, deltaTime);
            }
            if (glfwGetKey(m_glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
            {
                mainCamera->ProcessKeyboard(Camera::ECameraMovement::LEFT, deltaTime);
            }
            if (glfwGetKey(m_glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
            {
                mainCamera->ProcessKeyboard(Camera::ECameraMovement::RIGHT, deltaTime);
            }
            if (glfwGetKey(m_glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                mainCamera->ProcessKeyboard(Camera::ECameraMovement::UP, deltaTime);
            }
        }
        std::shared_ptr<RendererSource> rendererSource;
        if (!m_renderer->TryGetRendererSource(m_renderer->viewMode, rendererSource))
        {
            LOGF("Editor", "Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(m_renderer->viewMode)])
        }
        rendererSource->viewAndProjection->view = mainCamera->GetViewMatrix();
        auto projection                         = mainCamera->GetProjectionMatrix();
        projection[1][1] *= -1;
        rendererSource->viewAndProjection->projection = projection;

        if (ImGui::IsKeyPressed(ImGuiKey_G))
            m_currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R))
            m_currentGizmoOperation = ImGuizmo::ROTATE;
        // TODO:
        // if (ImGui::IsKeyPressed(ImGuiKey_S))
        // m_currentGizmoOperation = ImGuizmo::SCALE;


        // if (ImGui::IsKeyPressed(ImGuiKey_X))
        //     m_useSnap = !m_useSnap;
        // ImGui::Checkbox("##UseSnap", &m_useSnap);
        // ImGui::SameLine();
        //
        // switch (m_currentGizmoOperation)  // NOLINT(clang-diagnostic-switch, clang-diagnostic-switch-enum)
        // {
        //     case ImGuizmo::TRANSLATE:
        //         ImGui::InputFloat3("Snap", &m_snap[0]);
        //         break;
        //     case ImGuizmo::ROTATE:
        //         ImGui::InputFloat("Angle Snap", &m_snap[0]);
        //         break;
        //     case ImGuizmo::SCALE:
        //         ImGui::InputFloat("Scale Snap", &m_snap[0]);
        //         break;
        //     default:
        //         break;
        // }
    }
}


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

void Sandbox::Viewport::SwitchCursorForCameraMovement(GLFWwindow* inWindow, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_RIGHT || mainCamera == nullptr)
    {
        return;
    }
    auto enabled = action == 1;
    if (inWindow != m_glfwWindow || !m_isMouseHoveringInnerRect)
    {
        DisableCameraMovement();
        return;
    }
    if (m_isCameraMovementEnabled == enabled)
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

// void Sandbox::Viewport::SetTarget(const std::shared_ptr<GameObject>& target)
// {
//     // m_referenceGameObject = target;
// }

void Sandbox::Viewport::DrawGizmo(VkExtent2D extent2D)
{
    if (m_imguiWindow == nullptr || mainCamera == nullptr)
    {
        return;
    }

    auto inspector = Inspector::GetInspector<Transform>();
    if (inspector != nullptr && inspector->target != nullptr)
    {
        auto lastPosition = inspector->target->transform->position;
        auto lastRotation = inspector->target->transform->rotation.GetEulerDegrees();
        auto lastScale    = inspector->target->transform->scale;

        float translation[3] = {lastPosition.x, lastPosition.y, lastPosition.z};
        float rotation[3]    = {lastRotation.x, lastRotation.y, lastRotation.z};
        float scale[3]       = {lastScale.x, lastScale.y, lastScale.z};
        float matrix[16];
        // LOGD("Editor", "lastPosition {}, lastRotation {}, lastScale {}", lastPosition.ToString(), lastRotation.ToString(), lastScale.ToString())
        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, matrix);

        glm::mat4    view            = mainCamera->GetViewMatrix();
        glm::mat4    projection      = mainCamera->GetProjectionMatrix();
        const float* cameraViewConst = glm::value_ptr(view);
        float        cameraView[16];
        std::copy(cameraViewConst, cameraViewConst + 16, cameraView);
        const float* cameraProjection = glm::value_ptr(projection);
        // glm::mat4    model            = inspector->target->transform->GetModelMatrix();
        // auto         matrixConst      = glm::value_ptr(model);
        // float        matrix[16];
        // std::copy(matrixConst, matrixConst + 16, matrix);
        // for (size_t i = 0; i < 16; ++i)
        // {
        //     if (matrix1[i] != matrix[i])
        //     {
        //         LOGF("Editor", "matrix1[{}] {} != matrix[{}] {}", i, matrix1[i], i, matrix[i])
        //     }
        // }

        static float bounds[]        = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
        static float boundsSnap[]    = {0.1f, 0.1f, 0.1f};
        static bool  boundSizing     = false;
        static bool  boundSizingSnap = false;
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        auto rectPosition = m_imguiWindow->InnerRect.Min;
        // TODO: 双显示器和 multi viewport 的情况下， rectPosition.x 为负数会导致 gizmo 无法显示
        ImGuizmo::SetRect(rectPosition.x + m_startPosition.x, rectPosition.y + m_startPosition.y, ToFloat(extent2D.width), ToFloat(extent2D.height));
        ImGuizmo::Manipulate(cameraView, cameraProjection, m_currentGizmoOperation, m_currentGizmoMode, matrix, nullptr, m_useSnap ? &m_snap[0] : nullptr,
                             boundSizing ? bounds : nullptr, boundSizingSnap ? boundsSnap : nullptr);

        ImGuizmo::DecomposeMatrixToComponents(matrix, translation, rotation, scale);
        inspector->target->transform->position = translation;
        inspector->target->transform->rotation = glm::quat(glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2])));
        inspector->target->transform->scale    = scale;

        // auto applyMatrix = [inspector](const float innerMatrix[16])
        // {
        //     inspector->target->transform->position = glm::vec3(innerMatrix[12], innerMatrix[13], innerMatrix[14]);
        //     auto xAxis                             = glm::length(glm::vec3(innerMatrix[0], innerMatrix[4], innerMatrix[8]));
        //     auto yAxis                             = glm::length(glm::vec3(innerMatrix[1], innerMatrix[5], innerMatrix[9]));
        //     auto zAxis                             = glm::length(glm::vec3(innerMatrix[2], innerMatrix[6], innerMatrix[10]));
        //     // 将 matrix 转为 glm::mat3
        //     auto rotation = glm::mat3(innerMatrix[0] / xAxis, innerMatrix[1] / yAxis, innerMatrix[2] / zAxis, innerMatrix[4] / xAxis, innerMatrix[5] / yAxis,
        //                               innerMatrix[6] / zAxis, innerMatrix[8] / xAxis, innerMatrix[9] / yAxis, innerMatrix[10] / zAxis);
        //     inspector->target->transform->rotation = glm::quat_cast(rotation);
        //     inspector->target->transform->scale    = glm::vec3(xAxis, yAxis, zAxis);
        // };
        // applyMatrix(matrix1);
        // static float camDistance = 8.f;
        // ImGuiIO& io = ImGui::GetIO();
        // float viewManipulateRight = io.DisplaySize.x;
        // float viewManipulateTop = 0;
        // ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

        // EditTransform(cameraView, cameraProjection, matrix, true);
    }

    DrawOverlay();
}

void Sandbox::Viewport::DrawOverlay()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    const float                              padding             = 10.0f;
    ImVec2                                   innerRectTLPosition = m_imguiWindow->InnerRect.GetTL();
    ImVec2                                   innerRectSize       = m_imguiWindow->InnerRect.GetSize();
    const std::map<EViewportOverlay, ImVec2> overlayPositions    = {
        {EViewportOverlay::LEFT_TOP, ImVec2(innerRectTLPosition.x + padding, innerRectTLPosition.y + padding)},
        {EViewportOverlay::RIGHT_TOP, ImVec2(innerRectTLPosition.x + innerRectSize.x - padding, innerRectTLPosition.y + padding)},
    };
    const std::map<EViewportOverlay, ImVec2> overlayPivots = {
        {EViewportOverlay::LEFT_TOP, ImVec2(0.0f, 0.0f)},
        {EViewportOverlay::RIGHT_TOP, ImVec2(1.0f, 0.0f)},
    };
    // 先保存当前的背景颜色
    ImVec4 prevChildBg = ImGui::GetStyle().Colors[ImGuiCol_ChildBg];

    // 设置新的背景颜色和透明度
    ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = ImVec4(prevChildBg.x, prevChildBg.y, prevChildBg.z, 0.35f);  // 设置为半透明

    // 设置子窗口的起始位置
    ImGui::SetNextWindowPos(overlayPositions.at(LEFT_TOP), ImGuiCond_Always, overlayPivots.at(LEFT_TOP));

    auto childFlags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY;
    if (ImGui::BeginChild("ViewportOverlayLT", ImVec2(0, 0), childFlags, windowFlags))
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
    }
    ImGui::EndChild();

    ImGui::SetNextWindowPos(overlayPositions.at(RIGHT_TOP), ImGuiCond_Always, overlayPivots.at(RIGHT_TOP));
    if (ImGui::BeginChild("ViewportOverlayRT", ImVec2(0, 0), childFlags, windowFlags))
    {
        // UV coordinates are often (0.0f, 0.0f) and (1.0f, 1.0f) to display an entire textures.
        // Here are trying to display only a 32x32 pixels area of the texture, hence the UV computation.
        // Read about UV coordinates here: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImVec2 size                    = ImVec2(32.0f, 32.0f);  // Size of the image we want to make visible
        ImVec2 uv0                     = ImVec2(0.0f, 0.0f);  // UV coordinates for lower-left
        ImVec2 uv1                     = ImVec2(1.0f, 1.0f);
        ImVec4 backgroundColor         = ImVec4(0.176f, 0.176f, 0.176f, 0.2f);  // gray
        ImVec4 selectedBackgroundColor = ImVec4(0.04f, 0.35f, 0.8f, 0.8f);  // blue
        auto   translateTextureId      = (ImTextureID)ImGuiRenderer::guiNameToTextureId["translate"];
        if (ImGui::ImageButton("translate", translateTextureId, size, uv0, uv1, m_currentGizmoOperation == ImGuizmo::TRANSLATE ? selectedBackgroundColor : backgroundColor))
        {
            m_currentGizmoOperation = ImGuizmo::TRANSLATE;
        }
        ImGui::SameLine();
        auto rotateTextureId = (ImTextureID)ImGuiRenderer::guiNameToTextureId["rotate"];
        if (ImGui::ImageButton("rotate", rotateTextureId, size, uv0, uv1, m_currentGizmoOperation == ImGuizmo::ROTATE ? selectedBackgroundColor : backgroundColor))
        {
            m_currentGizmoOperation = ImGuizmo::ROTATE;
        }
        ImGui::SameLine();
        auto scaleTextureId = (ImTextureID)ImGuiRenderer::guiNameToTextureId["scale"];
        if (ImGui::ImageButton("scale", scaleTextureId, size, uv0, uv1, m_currentGizmoOperation == ImGuizmo::SCALE ? selectedBackgroundColor : backgroundColor))
        {
            m_currentGizmoOperation = ImGuizmo::SCALE;
        }
        // if (ImGui::RadioButton("Universal", m_currentGizmoOperation == ImGuizmo::UNIVERSAL))
        //     m_currentGizmoOperation = ImGuizmo::UNIVERSAL;
        ImGui::SameLine();
        auto gizmoLocalTextureId = (ImTextureID)ImGuiRenderer::guiNameToTextureId["local"];
        auto gizmoWorldTextureId = (ImTextureID)ImGuiRenderer::guiNameToTextureId["world"];
        auto gizmoModeTextureId  = m_currentGizmoMode == ImGuizmo::LOCAL ? gizmoLocalTextureId : gizmoWorldTextureId;
        if (m_currentGizmoOperation == ImGuizmo::SCALE)
        {
            gizmoModeTextureId = gizmoLocalTextureId;
        }
        if (ImGui::ImageButton("gizmoMode", gizmoModeTextureId, size, uv0, uv1, backgroundColor))
        {
            m_currentGizmoMode = m_currentGizmoMode == ImGuizmo::LOCAL ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
        }
        ImGui::SameLine();
        ImGui::PopStyleVar();
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
void Sandbox::Viewport::InspectTarget(std::shared_ptr<GameObject> inTarget) {}

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
        adjustedWidth     = viewportWidth;
        adjustedHeight    = ToUInt32(ToFloat(adjustedHeight) * scaleFactor);
        // 重新计算起始Y坐标
        startY = (viewportHeight - ToInt32(adjustedHeight)) / 2;
    }
    // 如果实际宽高比等于目标宽高比，起始位置保持为0，0，不需要计算

    return ImVec2(ToFloat(startX), ToFloat(startY));
}


void Sandbox::Viewport::BindCameraPosition(EViewMode inViewMode)
{
    std::shared_ptr<RendererSource> rendererSource;
    if (m_renderer->TryGetRendererSource(inViewMode, rendererSource))
    {
        // TODO:因为这里 position 是 Vector3 派生自 rfk::Object 虚函数表指针要求 8 字节对齐，因此不满足传递给 GPU 对齐要求
        if (mainCamera == nullptr)
        {
            rendererSource->pipeline->pipelineState->pushConstantsInfo.data = nullptr;
        }
        else
        {
            auto gameObject = mainCamera->gameObject.lock();
            rendererSource->pipeline->pipelineState->pushConstantsInfo.data = &gameObject->transform->position.vec;
        }
    }
}