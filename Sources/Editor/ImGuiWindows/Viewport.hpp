#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Editor/IImGuiWindow.hpp"
#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "Engine/RendererSource/StencilRendererSource.hpp"
#include "Generated/Viewport.rfkh.h"
#include "Misc/Delegate.hpp"
#include "VulkanRHI/Common/ViewMode.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"

struct ImGuiWindow;

namespace Sandbox NAMESPACE()
{
    class GameObject;
    class Camera;
    class Renderer;
    class Sampler;

    class CLASS() Viewport : public IImGuiWindow
    {
        enum EViewportOverlay
        {
            NONE,
            LEFT_TOP,
            RIGHT_TOP,
        };

    public:
        void OnCursorPosition(GLFWwindow* window, double xPos, double yPos);

        Viewport(const std::shared_ptr<Renderer>& inRenderer);

        void Prepare() override;

        void OnGui() override;

        void Tick(float deltaTime) override;

        void SwitchCursorForCameraMovement(GLFWwindow* inWindow, int button, int action, int mods);

        void DisableCameraMovement();

        void Cleanup() override;

        // void SetTarget(const std::shared_ptr<GameObject>& target);

        void DrawGizmo(VkExtent2D extent2D);
        void DrawOverlay();

        void OnRecreateFramebuffer();
        // void OnViewModeChanged(EViewMode inViewMode);

        void SelectObject();

        void InspectTarget(std::shared_ptr<GameObject> inTarget);

        std::vector<VkDescriptorSet> presentDescriptorSets;
        std::shared_ptr<Sampler>     presentSampler;

        std::shared_ptr<Camera> mainCamera;

        VkExtent2D                 resolvedResolution;
        // std::shared_ptr<ImageView> outputImageView;


    private:
        // std::shared_ptr<ImageView> m_lastOutputImageView;

        std::vector<VkDescriptorSet> m_lastPresentDescriptorSets;
        ImVec2 CalculateStartPosition(int aspectWidth, int aspectHeight, int resolutionWidth, int resolutionHeight, uint32_t& adjustedWidth, uint32_t& adjustedHeight);
        void   OnViewModeChanged(EViewMode inViewMode);
        void   OnAfterDrawMesh(const std::shared_ptr<CommandBuffer>&, uint32_t, std::shared_ptr<Mesh>&);
        // std::shared_ptr<GameObject> m_referenceGameObject;
        ImVec2                    m_startPosition;
        bool                      m_isMouseHoveringInnerRect;
        bool                      m_mouseMoved              = true;
        float                     m_lastX                   = 0.0f;
        float                     m_lastY                   = 0.0f;
        bool                      m_isCameraMovementEnabled = false;
        Sandbox::DelegateHandle   m_cursorBindHandler;
        GLFWwindow*               m_glfwWindow;
        ImGuiWindow*              m_imguiWindow;
        std::shared_ptr<Renderer> m_renderer;
        FIELD()
        ImGuizmo::OPERATION m_currentGizmoOperation;
        FIELD()
        ImGuizmo::MODE m_currentGizmoMode;
        bool           m_useSnap = false;
        float          m_snap[3] = {1.f, 1.f, 1.f};

        std::shared_ptr<StencilRendererSource> m_stencilRendererSource;

        std::shared_ptr<JPH::AllHitCollisionCollector<JPH::CastRayCollector>> m_selectionCollector;
        std::shared_ptr<JPH::BodyID>                                          m_selectedBodyId;
        std::shared_ptr<GameObject>                                           m_selectedGameObject;
        Sandbox_Viewport_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_Viewport_GENERATED
