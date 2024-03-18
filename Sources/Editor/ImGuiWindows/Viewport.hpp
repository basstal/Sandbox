﻿#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Editor/IImGuiWindow.hpp"
#include "GLFW/glfw3.h"
#include "Misc/Delegate.hpp"


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

        std::vector<VkDescriptorSet> presentDescriptorSets;
        std::shared_ptr<Sampler> presentSampler;

        std::shared_ptr<Camera> mainCamera;

    private:
        std::shared_ptr<GameObject> m_referenceGameObject;

        bool m_isMouseHoveringInnerRect;
        bool m_mouseMoved = true;
        float m_lastX = 0.0f;
        float m_lastY = 0.0f;
        bool m_isCameraMovementEnabled = false;
        Sandbox::DelegateHandle m_cursorBindHandler;
        GLFWwindow* m_glfwWindow;
        std::shared_ptr<Renderer> m_renderer;
    };
}
