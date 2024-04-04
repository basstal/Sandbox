#include "pch.hpp"

#include "Editor.hpp"

#include "Engine/Camera.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Engine/Timer.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"
#include "Grid.hpp"
#include "ImGuiRenderer.hpp"
#include "ImGuiWindows/Viewport.hpp"
#include "Misc/DataBinding.hpp"
#include "Platform/GlfwCallbackBridge.hpp"
#include "Platform/Window.hpp"
#include "TransformGizmo.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Core/Surface.hpp"
#include "VulkanRHI/Core/Swapchain.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

static Sandbox::File editorCameraConfigCache = Sandbox::Directory::GetLibraryDirectory().GetFile("EditorCamera.yaml");

void Sandbox::Editor::Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Timer>& timer, const std::vector<std::shared_ptr<Models>>& inModels,
                              const std::shared_ptr<Window>& inWindow)
{
    m_renderer    = renderer;
    m_timer       = timer;
    window        = inWindow;
    models        = inModels;
    imGuiRenderer = std::make_shared<ImGuiRenderer>();
    imGuiRenderer->Prepare(renderer, shared_from_this());

    auto&           resolution       = renderer->resolution;
    auto            aspectRatio      = static_cast<float>(resolution.width) / static_cast<float>(resolution.height);
    const glm::vec3 DEFAULT_WORLD_UP = glm::vec3(0.0f, 0.0f, 1.0f);
    // TODO:将 camera 移到 viewport 内
    camera = std::make_shared<Camera>(DEFAULT_WORLD_UP, aspectRatio);
    camera->LoadFromFile(editorCameraConfigCache);
    camera->UpdateCameraVectors();
    imGuiRenderer->viewport->mainCamera = camera;
    BindCameraPosition(renderer->viewMode);
    grid = std::make_shared<Grid>();
    grid->Prepare(renderer, shared_from_this());
    renderer->onBeforeRendererDraw.BindMember<Grid, &Grid::DrawGrid>(grid);
    renderer->onViewModeChanged.BindMember<Editor, &Editor::BindCameraPosition>(this);

    PrepareOnGui();
}

void Sandbox::Editor::BindCameraPosition(EViewMode inViewMode)
{
    std::shared_ptr<RendererSource> rendererSource;
    if (m_renderer->TryGetRendererSource(inViewMode, rendererSource))
    {
        // TODO:因为这里 position 是 Vector3 派生自 rfk::Object 虚函数表指针要求 8 字节对齐，因此不满足传递给 GPU 对齐要求
        rendererSource->pipeline->pipelineState->pushConstantsInfo.data = &camera->position.vec;
    }
}
void Sandbox::Editor::Cleanup()
{
    camera->SaveToFile(editorCameraConfigCache);
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
    auto         device = m_renderer->device;
    ShaderSource vertexShaderSource(Directory::GetAssetsDirectory().GetFile("Shaders/FillModeNonSolidGrid.vert"));
    shaderModules.push_back(std::make_shared<ShaderModule>(device, vertexShaderSource, "", VK_SHADER_STAGE_VERTEX_BIT));
    ShaderSource fragmentShaderSource(Directory::GetAssetsDirectory().GetFile("Shaders/FillModeNonSolidGrid.frag"));
    shaderModules.push_back(std::make_shared<ShaderModule>(device, fragmentShaderSource, "", VK_SHADER_STAGE_FRAGMENT_BIT));

    pipelineLayout     = std::make_shared<PipelineLayout>(device, shaderModules, std::vector<uint32_t>{1});
    auto renderPass    = m_renderer->renderPass;
    pipelineLineList   = std::make_shared<Pipeline>(device, shaderModules, renderPass, pipelineLayout, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL);
    auto pipelineState = std::make_shared<PipelineState>(shaderModules, renderPass, pipelineLayout);
    pipelineState->depthStencilState.depthTestEnable  = VK_FALSE;
    pipelineState->depthStencilState.depthWriteEnable = VK_TRUE;
    pipelineGizmo                                     = std::make_shared<Pipeline>(device, pipelineState);
    // auto pipelineState1 = std::make_shared<PipelineState>(shaderModules, renderPass, pipelineLayout);
    // pipelineState1->inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    // pipelineState1->depthStencilState.depthTestEnable = VK_TRUE;
    // pipelineState1->depthStencilState.depthWriteEnable = VK_TRUE;
    // pipelineGizmoDebug = std::make_shared<Pipeline>(device, pipelineState1);

    UpdateDescriptorSets(m_renderer->viewMode);
    m_renderer->onViewModeChanged.BindMember<Editor, &Editor::UpdateDescriptorSets>(this);
}

void Sandbox::Editor::UpdateDescriptorSets(EViewMode inViewMode)
{
    std::shared_ptr<RendererSource> rendererSource;
    if (!m_renderer->TryGetRendererSource(inViewMode, rendererSource))
    {
        LOGF("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(inViewMode)])
    }
    auto     uniformMvpObjects = rendererSource->uboMvp;
    uint32_t dynamicAlignment  = GetUniformDynamicAlignment(sizeof(glm::mat4));
    for (auto& descirptorSet : descriptorSets)
    {
        descirptorSet->Cleanup();
    }
    descriptorSets.clear();
    for (size_t i = 0; i < m_renderer->maxFramesFlight; ++i)
    {
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uniformMvpObjects[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uniformMvpObjects[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
        };
        BindingMap<VkDescriptorImageInfo> imageInfoMapping;
        auto                              descriptorSet =
            std::make_shared<DescriptorSet>(m_renderer->device, m_renderer->descriptorPool, pipelineLayout->descriptorSetLayout, bufferInfoMapping, imageInfoMapping);
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
    std::shared_ptr<RendererSource> rendererSource;
    if (!m_renderer->TryGetRendererSource(m_renderer->viewMode, rendererSource))
    {
        LOGF("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(m_renderer->viewMode)])
    }
    rendererSource->viewAndProjection->view = camera->GetViewMatrix();

    auto projection = camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    rendererSource->viewAndProjection->projection = projection;

    auto glfwWindow = window->glfwWindow;

    onUpdateInputs.Trigger(glfwWindow, camera, window->callbackBridge);
}
