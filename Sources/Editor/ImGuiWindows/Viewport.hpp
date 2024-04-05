#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Editor/IImGuiWindow.hpp"
#include "Misc/Delegate.hpp"
#include "VulkanRHI/Common/ViewMode.hpp"

struct ImGuiWindow;

namespace Sandbox
{
    class GameObject;
    class Camera;
    class Renderer;
    class Sampler;

    class Viewport : public IImGuiWindow
    {
    public:
        void OnCursorPosition(GLFWwindow* window, double xPos, double yPos);

        Viewport(const std::shared_ptr<Renderer>& inRenderer);

        void Prepare() override;

        void OnGui() override;

        void Tick(float deltaTime) override;

        void SwitchCursorForCameraMovement(GLFWwindow* inWindow, int button, int action, int mods);

        void DisableCameraMovement();

        void Cleanup() override;

        void SetTarget(const std::shared_ptr<GameObject>& target);

        void DrawGizmo(VkExtent2D extent2D);

        void OnRecreateFramebuffer();

        void InspectTarget(std::shared_ptr<GameObject> inTarget);

        std::vector<VkDescriptorSet> presentDescriptorSets;
        std::shared_ptr<Sampler>     presentSampler;

        std::shared_ptr<Camera> mainCamera;

    private:
        ImVec2 CalculateStartPosition(int aspectWidth, int aspectHeight, int resolutionWidth, int resolutionHeight, uint32_t& adjustedWidth, uint32_t& adjustedHeight);
        void   BindCameraPosition(EViewMode inViewMode);
        std::shared_ptr<GameObject> m_referenceGameObject;
        ImVec2                      m_startPosition;
        bool                        m_isMouseHoveringInnerRect;
        bool                        m_mouseMoved              = true;
        float                       m_lastX                   = 0.0f;
        float                       m_lastY                   = 0.0f;
        bool                        m_isCameraMovementEnabled = false;
        Sandbox::DelegateHandle     m_cursorBindHandler;
        GLFWwindow*                 m_glfwWindow;
        ImGuiWindow*                m_imguiWindow;
        std::shared_ptr<Renderer>   m_renderer;
    };
}  // namespace Sandbox
