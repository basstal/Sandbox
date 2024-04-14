#include "pch.hpp"

#include "RendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Components/Transform.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Engine/PhysicsSystem.hpp"
#include "Misc/Memory.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

void Sandbox::RendererSource::Prepare(std::shared_ptr<Renderer>& renderer)
{
    uboMvp.resize(renderer->maxFramesFlight);
    viewAndProjection = std::make_shared<ViewAndProjection>();
    for (size_t i = 0; i < renderer->maxFramesFlight; ++i)
    {
        uboMvp[i] = PrepareUniformBuffers(renderer);
    }
    CreatePipeline(renderer);
    CreateDescriptorSets(renderer);
}


std::shared_ptr<Sandbox::MVPUboObjects> Sandbox::RendererSource::PrepareUniformBuffers(std::shared_ptr<Renderer>& renderer)
{
    auto mvpUboObjects   = std::make_shared<MVPUboObjects>();
    mvpUboObjects->vpUbo = std::make_shared<UniformBuffer>(renderer->device, sizeof(ViewAndProjection));
    mvpUboObjects->vpUbo->Update(reinterpret_cast<void*>(viewAndProjection.get()));

    // Calculate required alignment based on minimum device offset alignment
    VkDeviceSize dynamicAlignment = sizeof(glm::mat4);
    renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    constexpr uint32_t objectCount      = 2;
    VkDeviceSize       modelsBufferSize = objectCount * dynamicAlignment;

    mvpUboObjects->modelsUbo = std::make_shared<UniformBuffer>(renderer->device, modelsBufferSize);
    return mvpUboObjects;
}

void Sandbox::RendererSource::RecreateUniformModels(const std::shared_ptr<Renderer>& renderer)
{
    size_t objectCount = Scene::currentScene->renderMeshes.size() + 1;
    // 修改 models 指向数组的大小并更新 descriptorSets
    for (size_t i = 0; i < uboMvp.size(); ++i)
    {
        uboMvp[i]->modelsUbo->Cleanup();

        // Calculate required alignment based on minimum device offset alignment
        VkDeviceSize dynamicAlignment = sizeof(glm::mat4);
        renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
        VkDeviceSize modelsBufferSize = objectCount * dynamicAlignment;

        uboMvp[i]->modelsUbo = std::make_shared<UniformBuffer>(renderer->device, modelsBufferSize);
    }
    UpdateDescriptorSets(renderer);
}


void Sandbox::RendererSource::UpdateModels(const std::shared_ptr<Renderer>& renderer, const std::vector<std::shared_ptr<Models>>& inModels,
                                           const std::vector<std::shared_ptr<Mesh>>& inMeshes)
{
    VkDeviceSize dynamicAlignment = sizeof(glm::mat4);
    renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    size_t       objectCount      = inMeshes.size() + 1;
    VkDeviceSize modelsBufferSize = objectCount * dynamicAlignment;

    for (size_t i = 0; i < uboMvp.size(); ++i)
    {
        inModels[i]->model = static_cast<glm::mat4*>(AlignedAlloc(modelsBufferSize, dynamicAlignment));
        size_t c           = 0;
        for (; c < inMeshes.size(); ++c)
        {
            auto transform        = inMeshes[c]->transform.lock();
            inModels[i]->model[c] = transform->GetModelMatrix();
        }
        inModels[i]->model[c] = glm::mat4(1.0f);  // NOTE:grid 一直处在场景中央
        uboMvp[i]->modelsUbo->Update(inModels[i]->model);
    }
}

void Sandbox::RendererSource::UpdateUniforms(uint32_t frameFlightIndex)
{
    SyncViewAndProjection();
    // static MVP
    uboMvp[frameFlightIndex]->vpUbo->Update(viewAndProjection.get());
}
void Sandbox::RendererSource::Cleanup()
{
    pipeline->Cleanup();
    pipelineLayout->Cleanup();
    for (auto& shaderModule : shaderModules)
    {
        shaderModule->Cleanup();
    }
    shaderModules.clear();
    for (size_t i = 0; i < uboMvp.size(); ++i)
    {
        uboMvp[i]->modelsUbo->Cleanup();
        uboMvp[i]->vpUbo->Cleanup();
    }
}

void Sandbox::RendererSource::CustomDrawMesh(const std::shared_ptr<Sandbox::Mesh>& mesh, const std::shared_ptr<Sandbox::CommandBuffer>& shared,
                                             const std::shared_ptr<Sandbox::DescriptorSet>& descriptorSet, uint32_t frameFlightIndex, uint32_t dynamicOffsets)
{
}

void Sandbox::RendererSource::CustomDrawOverlay(const std::shared_ptr<Sandbox::Mesh>& mesh, const std::shared_ptr<Sandbox::CommandBuffer>& shared,
                                                const std::shared_ptr<Sandbox::DescriptorSet>& descriptorSet, uint32_t frameFlightIndex, uint32_t dynamicOffsets)
{
}
void Sandbox::RendererSource::SyncViewAndProjection()
{
    if (camera == nullptr)
    {
        return;
    }
    viewAndProjection->view = camera->GetViewMatrix();
    auto projection         = camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    viewAndProjection->projection = projection;
}

void Sandbox::RendererSource::Tick(const std::shared_ptr<Renderer>& renderer)
{
    auto currentScene = Scene::GetCurrentScene();
    UpdateUniforms(renderer->frameFlightIndex);
    for (auto& mesh : currentScene->renderMeshes)
    {
        auto bodyId = mesh->GetPhysicsBodyId();
        PhysicsSystem::Instance->SyncTransform(bodyId, mesh->transform.lock());
    }
    UpdateModels(renderer, currentScene->models, currentScene->renderMeshes);
    uboMvp[renderer->frameFlightIndex]->modelsUbo->Update(currentScene->models[renderer->frameFlightIndex]->model);
}

void Sandbox::RendererSource::SetCamera(const std::shared_ptr<Camera>& inCamera){
    camera = inCamera;
}
