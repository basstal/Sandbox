#include "pch.hpp"

#include "RendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
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
    constexpr uint32_t objectCount      = 1 + 2;
    VkDeviceSize       modelsBufferSize = objectCount * dynamicAlignment;

    mvpUboObjects->modelsUbo = std::make_shared<UniformBuffer>(renderer->device, modelsBufferSize);
    return mvpUboObjects;
}


void Sandbox::RendererSource::UpdateModels(const std::shared_ptr<Renderer>& renderer, const std::vector<std::shared_ptr<Models>>& inModels)
{
    VkDeviceSize dynamicAlignment = sizeof(glm::mat4);
    renderer->device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    constexpr uint32_t objectCount      = 1 + 2;
    VkDeviceSize       modelsBufferSize = objectCount * dynamicAlignment;

    for (size_t i = 0; i < uboMvp.size(); ++i)
    {
        // TODO: dynamic model value from scene objects
        inModels[i]->model    = static_cast<glm::mat4*>(AlignedAlloc(modelsBufferSize, dynamicAlignment));
        inModels[i]->model[0] = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
        inModels[i]->model[1] = glm::mat4(1.0f);  // NOTE:grid 一直处在场景中央
        uboMvp[i]->modelsUbo->Update(inModels[i]->model);
    }
}

void Sandbox::RendererSource::UpdateUniforms(uint32_t frameFlightIndex)
{
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
