#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Misc/Event.hpp"
#include "VulkanRHI/Common/ViewMode.hpp"


namespace Sandbox
{
    class ShaderModule;
    class Grid;
    class Timer;
    class Camera;
    class Renderer;
    class ImGuiRenderer;
    class DescriptorSet;
    struct Models;
    class PipelineLayout;
    class Pipeline;
    class GlfwCallbackBridge;
    class TransformGizmo;
    class Window;


    class Editor : public std::enable_shared_from_this<Editor>
    {
    public:
        void Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Timer>& timer, const std::vector<std::shared_ptr<Models>>& inModels,
                     const std::shared_ptr<Window>& inWindow);
        void BindCameraPosition(EViewMode inViewMode);

        void Draw();

        void Update();


        void UpdateInputs(const std::shared_ptr<GlfwCallbackBridge>& glfwInputBridge);

        void Cleanup();

        void CleanupOnGui();

        void PrepareOnGui();
        void UpdateDescriptorSets(EViewMode inViewMode);

        uint32_t GetUniformDynamicAlignment(VkDeviceSize dynamicAlignment) const;

        std::shared_ptr<ImGuiRenderer> imGuiRenderer;
        std::shared_ptr<Grid>          grid;
        std::shared_ptr<Camera>        camera;

        std::vector<std::shared_ptr<ShaderModule>>  shaderModules;
        std::shared_ptr<PipelineLayout>             pipelineLayout;
        std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
        // std::shared_ptr<TransformGizmo> transformGizmo;
        std::vector<std::shared_ptr<Models>> models;
        std::shared_ptr<Pipeline>            pipelineLineList;
        std::shared_ptr<Pipeline>            pipelineGizmo;
        // std::shared_ptr<Pipeline> pipelineGizmoDebug;
        Event<GLFWwindow*, const std::shared_ptr<Camera>&, const std::shared_ptr<GlfwCallbackBridge>&> onUpdateInputs;
        std::shared_ptr<Window>                                                                        window;

    private:
        std::shared_ptr<Timer>    m_timer;
        std::shared_ptr<Renderer> m_renderer;
    };
}  // namespace Sandbox
