#pragma once
#include <array>
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include <GLFW/glfw3.h>

namespace Sandbox
{
    class BoundingBox;
    class Ray;
    class GlfwCallbackBridge;
    class Camera;
    class Buffer;
    class Editor;
    struct SimpleVertex;
    class GameObject;
    class RenderAttachments;
    class RenderTarget;
    class CommandBuffer;
    class Renderer;

    class TransformGizmo
    {
    public:
        void Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Editor>& editor);

        void CreateFramebuffer();

        void UpdateGizmoBoundingBoxes(const glm::mat4& modelToWorld);

        void DrawGizmo(const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t frameFlightIndex);

        void SetTarget(const std::shared_ptr<GameObject>& target);

        void Cleanup();

        void UpdateInputs(GLFWwindow* window, const std::shared_ptr<Camera>& camera, const std::shared_ptr<GlfwCallbackBridge>& glfwInputBridge);

        void UpdateGizmoAndObjectPosition(GLFWwindow* window, const std::shared_ptr<Camera>& camera);

        std::shared_ptr<Ray> cameraRay;

    private:
        std::vector<SimpleVertex> m_vertices;
        std::shared_ptr<Buffer> m_vertexBuffer;
        bool m_prepared = false;
        std::shared_ptr<GameObject> m_referenceGameObject;
        std::shared_ptr<Editor> m_editor;

        bool m_gizmoActiveX = false;
        bool m_gizmoActiveY = false;
        bool m_gizmoActiveZ = false;
        std::vector<Sandbox::SimpleVertex> m_arrowX;
        std::vector<Sandbox::SimpleVertex> m_arrowY;
        std::vector<Sandbox::SimpleVertex> m_arrowZ;
        std::shared_ptr<RenderAttachments> m_renderAttachments;
        /**
         * \brief  Gizmo 的 包围盒
         */
        std::array<std::shared_ptr<BoundingBox>, 3> m_gizmoBoundingBoxes = std::array<std::shared_ptr<BoundingBox>, 3>();
        float m_previousScaleFactor = 0;
        std::shared_ptr<Renderer> m_renderer;
        std::shared_ptr<RenderTarget> m_renderTarget;
    };
}
