#include "Viewport.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_vulkan.h"
#include "Editor/ImGuiExamples.hpp"
#include "Engine/Camera.hpp"
#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/EntityComponent/Components/Transform.hpp"
#include "ImGuizmo.h"
#include "FileSystem/Logger.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Misc/GlmExtensions.hpp"
#include "Misc/TypeCasting.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Sampler.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"


void Sandbox::Viewport::OnCursorPosition(GLFWwindow* window, double xPos, double yPos)
{
    if (m_mouseMoved)
    {
        m_lastX = static_cast<float>(xPos);
        m_lastY = static_cast<float>(yPos);
        m_mouseMoved = false;
    }

    float offsetX = static_cast<float>(xPos) - m_lastX;
    float offsetY = m_lastY - static_cast<float>(yPos);
    m_lastX = static_cast<float>(xPos);
    m_lastY = static_cast<float>(yPos);
    mainCamera->ProcessMouseMovement(offsetX, offsetY);
}

Sandbox::Viewport::Viewport(const std::shared_ptr<Renderer>& inRenderer)
{
    name = "viewport";
    flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    m_renderer = inRenderer;
    presentSampler = std::make_shared<Sampler>(m_renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    m_renderer->swapchain->onAfterRecreateSwapchain.BindMember<Viewport, &Viewport::OnRecreateFramebuffer>(this);
    m_renderer->surface->window->callbackBridge->onMouseButton.BindMember<Viewport, &Viewport::SwitchCursorForCameraMovement>(this);
}

void Sandbox::Viewport::Prepare()
{
    OnRecreateFramebuffer();
}

void Sandbox::Viewport::OnGui()
{
    m_renderer->Draw();
    // ImVec2 windowPos = ImGui::GetWindowPos();
    // ImVec2 windowSize = ImGui::GetWindowSize();
    auto imageIndex = m_renderer->swapchain->acquiredNextImageIndex;
    auto extent2D = m_renderer->swapchain->vkExtent2D;
    ImGui::Image((ImTextureID)presentDescriptorSets[imageIndex], ImVec2(ToFloat(extent2D.width), ToFloat(extent2D.height)));
    auto window = ImGui::GetCurrentWindow();
    m_glfwWindow = reinterpret_cast<GLFWwindow*>(window->Viewport->PlatformHandle);
    m_isMouseHoveringInnerRect = ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
    flags = ImGui::IsWindowHovered() && m_isMouseHoveringInnerRect ? flags | ImGuiWindowFlags_NoMove : flags ^ ImGuiWindowFlags_NoMove;
    if ((flags & ImGuiWindowFlags_NoMove) != 0)
    {
        ImGui::SetNextFrameWantCaptureMouse(true);
    }
    // auto ray = std::make_shared<Ray>(CursorPositionToWorldRay(m_renderer->surface->window->glfwWindow, m_editor->camera->GetViewMatrix(),
    //                                                           m_editor->camera->GetProjectionMatrix()));
    // LOGD(ray->ToString())
    // if (m_editor->transformGizmo->cameraRay)
    // {
    //     m_editor->transformGizmo->cameraRay->origin = m_editor->camera->property->position;
    //     LOGD("camera front : {}", Sandbox::ToString(m_editor->camera->front))
    //     m_editor->transformGizmo->cameraRay->direction = ray->direction;
    // }
    // else
    // {
    //     m_editor->transformGizmo->cameraRay = ray;
    // }
    // m_editor->transformGizmo->cameraRay->PrepareDebugDrawData(m_renderer->device);
    DrawGizmo(extent2D);
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
    if (!m_isCameraMovementEnabled) // 如果之前没有开启
    {
        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_cursorBindHandler = window->callbackBridge->onCursorPosition.BindMember<Viewport, &Viewport::OnCursorPosition>(this);
        m_isCameraMovementEnabled = true;
    }
    else // 如果之前是开启的，现在要关闭
    {
        DisableCameraMovement();
    }
}

void Sandbox::Viewport::DisableCameraMovement()
{
    auto window = m_renderer->surface->window;
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    window->callbackBridge->onCursorPosition.Unbind(m_cursorBindHandler);
    m_cursorBindHandler = Sandbox::DelegateHandle::Null;
    m_mouseMoved = true;
    m_isCameraMovementEnabled = false;
}

void Sandbox::Viewport::Cleanup()
{
    presentSampler->Cleanup();

    for (auto& presentDescriptorSet : presentDescriptorSets)
    {
        ImGui_ImplVulkan_RemoveTexture(presentDescriptorSet);
    }
}

void Sandbox::Viewport::SetTarget(const std::shared_ptr<GameObject>& target)
{
    m_referenceGameObject = target;
}

void Sandbox::Viewport::DrawGizmo(VkExtent2D extent2D)
{
    if (m_referenceGameObject == nullptr || m_glfwWindow == nullptr)
    {
        return;
    }
    glm::mat4 model = m_referenceGameObject->transform->GetModelMatrix();
    glm::mat4 view = mainCamera->GetViewMatrix();
    glm::mat4 projection = mainCamera->GetProjectionMatrix();
    float const* cameraViewConst = glm::value_ptr(view);
    float cameraView[16];
    std::copy(cameraViewConst, cameraViewConst + 16, cameraView);
    float const* cameraProjection = glm::value_ptr(projection);
    auto matrixConst = glm::value_ptr(model);
    float matrix[16];
    std::copy(matrixConst, matrixConst + 16, matrix);

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = {1.f, 1.f, 1.f};
    static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
    static bool boundSizing = false;
    static bool boundSizingSnap = false;
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    int xPos, yPos;
    glfwGetWindowPos(m_glfwWindow, &xPos, &yPos);
    ImGuizmo::SetRect(ToFloat(xPos), ToFloat(yPos), ToFloat(extent2D.width), ToFloat(extent2D.height));
    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, nullptr, useSnap ? &snap[0] : nullptr, boundSizing ? bounds : nullptr,
                         boundSizingSnap ? boundsSnap : nullptr);

    m_referenceGameObject->transform->position = glm::vec3(matrix[12], matrix[13], matrix[14]);

    // static float camDistance = 8.f;
    // ImGuiIO& io = ImGui::GetIO();
    // float viewManipulateRight = io.DisplaySize.x;
    // float viewManipulateTop = 0;
    // ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

    // EditTransform(cameraView, cameraProjection, matrix, true);
}

void Sandbox::Viewport::OnRecreateFramebuffer()
{
    auto imageViewSize = m_renderer->swapchain->imageViews.size();
    presentDescriptorSets.resize(imageViewSize);
    for (size_t i = 0; i < imageViewSize; ++i)
    {
        VkImageView offlineImageView = m_renderer->renderAttachments[i]->resolveImageView->vkImageView; // Vulkan图像视图，已经创建并指向包含渲染结果的图像
        presentDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(presentSampler->vkSampler, offlineImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}
