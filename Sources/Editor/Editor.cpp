#include "Editor.hpp"

#include "Grid.hpp"
#include "ImGuiRenderer.hpp"
#include "TransformGizmo.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Timer.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"
#include "ImGuiWindows/Viewport.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Misc/DataBinding.hpp"
#include "Platform/Window.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"


void Sandbox::Editor::Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Timer>& timer, const std::vector<std::shared_ptr<Models>>& inModels,
                              const std::shared_ptr<Window>& inWindow)
{
    // editorWeakPtr = weak_from_this();
    m_renderer = renderer;
    m_timer = timer;
    window = inWindow;
    imGuiRenderer = std::make_shared<ImGuiRenderer>();
    imGuiRenderer->Prepare(m_renderer, shared_from_this());


    models = inModels;
    auto& extent2D = renderer->swapchain->vkExtent2D;
    auto aspectRatio = static_cast<float>(extent2D.width) / static_cast<float>(extent2D.height);
    auto cameraProperty = std::make_shared<CameraProperty>();
    // TODO:将 camera 移到 viewport 内
    camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 1.0f), aspectRatio, cameraProperty);
    imGuiRenderer->viewport->mainCamera = camera;
    renderer->pipeline->pipelineState->pushConstantsInfo.data = &camera->property->position;

    grid = std::make_shared<Grid>();
    grid->Prepare(m_renderer, shared_from_this());
    m_renderer->onBeforeRendererDraw.BindMember<Grid, &Grid::DrawGrid>(grid);

    // transformGizmo = std::make_shared<TransformGizmo>();
    // transformGizmo->Prepare(m_renderer, shared_from_this());
    // onUpdateInputs.BindMember<TransformGizmo, &TransformGizmo::UpdateInputs>(transformGizmo);
    // m_renderer->onAfterRendererDraw.BindMember<TransformGizmo, &TransformGizmo::DrawGizmo>(transformGizmo);
}

void Sandbox::Editor::Cleanup()
{
    imGuiRenderer->Cleanup();
    CleanupOnGui();
}


void Sandbox::Editor::CleanupOnGui()
{
    // transformGizmo->Cleanup();
    // transformGizmo = nullptr;
    grid->Cleanup();
    grid = nullptr;

    for (auto& descriptorSet : descriptorSets)
    {
        descriptorSet->Cleanup();
    }
    pipelineLayout->Cleanup();
    pipelineGizmo->Cleanup();
    pipelineLineList->Cleanup();
    for (auto& shaderModule : shaderModules)
    {
        shaderModule->Cleanup();
    }
}

void Sandbox::Editor::PrepareOnGui()
{
    auto device = m_renderer->device;
    ShaderSource vertexShaderSource(Directory::GetAssetsDirectory().GetFile("Shaders/FillModeNonSolidGrid.vert"));
    shaderModules.push_back(std::make_shared<ShaderModule>(device, vertexShaderSource, "", VK_SHADER_STAGE_VERTEX_BIT));
    ShaderSource fragmentShaderSource(Directory::GetAssetsDirectory().GetFile("Shaders/FillModeNonSolidGrid.frag"));
    shaderModules.push_back(std::make_shared<ShaderModule>(device, fragmentShaderSource, "", VK_SHADER_STAGE_FRAGMENT_BIT));

    pipelineLayout = std::make_shared<PipelineLayout>(device, shaderModules, std::vector<uint32_t>{1});
    auto renderPass = m_renderer->renderPass;
    pipelineLineList = std::make_shared<Pipeline>(device, shaderModules, renderPass, pipelineLayout, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL);
    auto pipelineState = std::make_shared<PipelineState>(shaderModules, renderPass, pipelineLayout);
    pipelineState->depthStencilState.depthTestEnable = VK_FALSE;
    pipelineState->depthStencilState.depthWriteEnable = VK_TRUE;
    pipelineGizmo = std::make_shared<Pipeline>(device, pipelineState);
    // auto pipelineState1 = std::make_shared<PipelineState>(shaderModules, renderPass, pipelineLayout);
    // pipelineState1->inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    // pipelineState1->depthStencilState.depthTestEnable = VK_TRUE;
    // pipelineState1->depthStencilState.depthWriteEnable = VK_TRUE;
    // pipelineGizmoDebug = std::make_shared<Pipeline>(device, pipelineState1);

    auto uniformMvpObjects = m_renderer->uboMvp;
    uint32_t dynamicAlignment = GetUniformDynamicAlignment(sizeof(glm::mat4));
    for (size_t i = 0; i < m_renderer->maxFramesFlight; ++i)
    {
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uniformMvpObjects[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uniformMvpObjects[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
        };
        auto descriptorSet = std::make_shared<DescriptorSet>(device, m_renderer->descriptorPool, pipelineLayout->descriptorSetLayout, bufferInfoMapping,
                                                             BindingMap<VkDescriptorImageInfo>());
        descriptorSets.push_back(descriptorSet);
    }
}

uint32_t Sandbox::Editor::GetUniformDynamicAlignment(VkDeviceSize dynamicAlignment) const
{
    m_renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    return static_cast<uint32_t>(dynamicAlignment);
}

void Sandbox::Editor::Draw()
{
    if (m_renderer->AcquireNextImage() != Continue)
    {
        return;
    }
    imGuiRenderer->Draw();
    m_renderer->Preset();
}

void Sandbox::Editor::Update()
{
    UpdateInputs(nullptr);
    float deltaTime = m_timer->GetDeltaTime();
    imGuiRenderer->Tick(deltaTime);
}


void Sandbox::Editor::UpdateInputs(const std::shared_ptr<GlfwCallbackBridge>& glfwInputBridge)
{
    m_renderer->viewAndProjection->view = camera->GetViewMatrix();
    auto projection = camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    m_renderer->viewAndProjection->projection = projection;

    auto glfwWindow = window->glfwWindow;

    onUpdateInputs.Trigger(glfwWindow, camera, window->callbackBridge);
}
