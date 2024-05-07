﻿#include "pch.hpp"

#include "Editor.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Engine/Timer.hpp"
#include "FileSystem/Directory.hpp"
#include "Grid.hpp"
#include "ImGuiRenderer.hpp"
#include "ImGuiWindows/Viewport.hpp"
#include "Misc/DataBinding.hpp"
#include "Misc/Debug.hpp"
#include "Platform/Window.hpp"
#include "TransformGizmo.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

void Sandbox::Editor::Prepare(const std::shared_ptr<Renderer>& renderer, const std::shared_ptr<Timer>& timer, const std::vector<std::shared_ptr<Models>>& inModels,
                              const std::shared_ptr<Window>& inWindow)
{
    m_renderer    = renderer;
    m_timer       = timer;
    window        = inWindow;
    models        = inModels;
    imGuiRenderer = std::make_shared<ImGuiRenderer>();
    imGuiRenderer->Prepare(renderer, shared_from_this());


    grid = std::make_shared<Grid>();
    grid->Prepare(renderer, shared_from_this());
    PrepareOnGui();
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
    // pipelineLayout->Cleanup();
    pipelineGizmo->Cleanup();
    pipelineLineList->Cleanup();
    // shaderLinkage->Cleanup();
    // for (auto& shaderModule : shaderLinkage)
    // {
    //     shaderModule->Cleanup();
    // }
}

void Sandbox::Editor::PrepareOnGui()
{
    auto device             = m_renderer->device;
    shaderLinkage           = std::make_shared<ShaderLinkage>();
    auto vertexShaderSource = std::make_shared<ShaderSource>(Directory::GetAssetsDirectory().GetFile("Shaders/FillModeNonSolidGrid.vert"), "");
    auto vertexShaderModule = shaderLinkage->LinkShaderModule(m_renderer, VK_SHADER_STAGE_VERTEX_BIT, vertexShaderSource);
    vertexShaderModule->SetUniformDescriptorMode("Model", Dynamic);
    auto fragmentShaderSource = std::make_shared<ShaderSource>(Directory::GetAssetsDirectory().GetFile("Shaders/FillModeNonSolidGrid.frag"), "");
    shaderLinkage->LinkShaderModule(m_renderer, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderSource);

    // pipelineLayout                                     = std::make_shared<PipelineLayout>(device, shaderModules);
    auto renderPass                                              = m_renderer->renderPass;
    auto pipelineStateLineList                                   = std::make_shared<PipelineState>(shaderLinkage, renderPass);
    pipelineStateLineList->multisampleState.rasterizationSamples = device->GetMaxUsableSampleCount();
    pipelineStateLineList->inputAssemblyState.topology           = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    pipelineLineList                                             = m_renderer->pipelineCaching->GetOrCreatePipeline(pipelineStateLineList);

    auto pipelineState                                   = std::make_shared<PipelineState>(shaderLinkage, renderPass);
    pipelineState->multisampleState.rasterizationSamples = device->GetMaxUsableSampleCount();
    pipelineState->depthStencilState.depthTestEnable     = VK_FALSE;
    pipelineState->depthStencilState.depthWriteEnable    = VK_TRUE;
    pipelineGizmo                                        = m_renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);
    // auto pipelineState1 = std::make_shared<PipelineState>(shaderModules, renderPass, pipelineLayout);
    // pipelineState1->inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    // pipelineState1->depthStencilState.depthTestEnable = VK_TRUE;
    // pipelineState1->depthStencilState.depthWriteEnable = VK_TRUE;
    // pipelineGizmoDebug = std::make_shared<Pipeline>(device, pipelineState1);

    for (size_t i = 0; i < m_renderer->maxFramesFlight; ++i)
    {
        auto descriptorSet = m_renderer->descriptorSetCaching->GetOrCreateDescriptorSet(pipelineLineList->pipelineLayout->descriptorSetLayout, i);
        descriptorSets.push_back(descriptorSet);
    }
    UpdateDescriptorSets(m_renderer->viewMode);
    m_renderer->onViewModeChanged.BindMember<Editor, &Editor::UpdateDescriptorSets>(this);
    Scene::onSceneChange.Bind(
        [this](const std::shared_ptr<Scene>& inScene)
        {
            auto camera = inScene->FindFirstCamera();
            Camera::onCameraChange.Trigger(camera);
            imGuiRenderer->onTargetChanged.Trigger(nullptr);
            UpdateDescriptorSets(m_renderer->viewMode);
        });
    Camera::onCameraChange.Bind(
        [this](const std::shared_ptr<Camera>& inCamera)
        {
            imGuiRenderer->viewport->SetMainCamera(inCamera);
            for (auto& [_, rendererSource] : m_renderer->rendererSourceMapping)
            {
                rendererSource->SetCamera(inCamera);
            }
        });
    Scene::onReconstructMeshes.Bind([this] { UpdateDescriptorSets(m_renderer->viewMode); });
}


void Sandbox::Editor::UpdateDescriptorSets(EViewMode inViewMode)
{
    std::shared_ptr<RendererSource> rendererSource;
    if (!m_renderer->TryGetRendererSource(inViewMode, rendererSource))
    {
        LOGF_OLD("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(inViewMode)])
    }
    auto     uniformMvpObjects = rendererSource->uboMvp;
    uint32_t dynamicAlignment  = GetUniformDynamicAlignment(sizeof(glm::mat4));
    // for (auto& descirptorSet : descriptorSets)
    // {
    //     descirptorSet->Cleanup();
    // }
    LOGD("Editor", "UpdateDescriptorSets\n{}", GetCallStack())
    // descriptorSets.clear();
    for (size_t i = 0; i < m_renderer->maxFramesFlight; ++i)
    {
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uniformMvpObjects[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uniformMvpObjects[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
        };
        descriptorSets[i]->BindBufferInfoMapping(bufferInfoMapping, pipelineLineList->pipelineLayout->descriptorSetLayout);
    }
}

uint32_t Sandbox::Editor::GetUniformDynamicAlignment(VkDeviceSize dynamicAlignment) const
{
    m_renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    return static_cast<uint32_t>(dynamicAlignment);
}

void Sandbox::Editor::Draw() { imGuiRenderer->Draw(); }

void Sandbox::Editor::Update()
{
    float deltaTime = m_timer->GetDeltaTime();
    imGuiRenderer->Tick(deltaTime);
}
