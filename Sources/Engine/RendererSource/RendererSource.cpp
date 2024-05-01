#include "pch.hpp"

#include "RendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Components/Transform.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Engine/PhysicsSystem.hpp"
#include "Engine/Skybox.hpp"
#include "FileSystem/Directory.hpp"
#include "Misc/Memory.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

void Sandbox::RendererSource::Prepare(std::shared_ptr<Renderer>& renderer)
{
    uboMvp.resize(renderer->maxFramesFlight);
    uboMvpNoMoving.resize(renderer->maxFramesFlight);
    viewAndProjection = std::make_shared<ViewAndProjection>();
    viewAndProjectionNoMoving = std::make_shared<ViewAndProjection>();
    for (size_t i = 0; i < renderer->maxFramesFlight; ++i)
    {
        uboMvp[i] = PrepareUniformBuffers(renderer);
        uboMvpNoMoving[i] = PrepareUniformBuffers(renderer);
    }
    auto resolution = renderer->resolution;
    outputImage     = std::make_shared<Image>(renderer->device, VkExtent3D{resolution.width, resolution.height, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                          VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 1);
    outputImageView = std::make_shared<ImageView>(outputImage, VK_IMAGE_VIEW_TYPE_2D);
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
    uboMvpNoMoving[frameFlightIndex]->vpUbo->Update(viewAndProjectionNoMoving.get());

}
void Sandbox::RendererSource::Cleanup()
{
    // pipeline->Cleanup();
    // pipelineLayout->Cleanup();
    // for (auto& shaderModule : shaderLinkage)
    // {
    //     shaderModule->Cleanup();
    // }
    // shaderLinkage.clear();
    outputImageView->Cleanup();
    outputImage->Cleanup();
    // shaderLinkage->Cleanup();
    for (size_t i = 0; i < uboMvp.size(); ++i)
    {
        uboMvp[i]->modelsUbo->Cleanup();
        uboMvp[i]->vpUbo->Cleanup();
        uboMvpNoMoving[i]->modelsUbo->Cleanup();
        uboMvpNoMoving[i]->vpUbo->Cleanup();
    }
}

void Sandbox::RendererSource::PushConstants(const std::shared_ptr<CommandBuffer>& inCommandBuffer) {}
void Sandbox::RendererSource::BindPipeline(const std::shared_ptr<CommandBuffer>& inCommandBuffer) {}

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
    viewAndProjectionNoMoving->view = glm::mat4(glm::mat3(viewAndProjection->view));
    auto projection         = camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    viewAndProjection->projection = projection;
    viewAndProjectionNoMoving->projection = projection;
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
    if (camera != nullptr && camera->skybox != nullptr)
    {
        if (!camera->skybox->IsPrepared())
        {
            camera->skybox->Prepare(renderer);
            std::vector<std::shared_ptr<Resource::Image>> images;
            for (auto& path : camera->skyboxImagePaths)
            {
                auto file = Directory::GetAssetsDirectory().GetFile(path);
                images.push_back(std::make_shared<Resource::Image>(file));
            }
            camera->skybox->CreateCubemapImages(images);
        }
    }
}

void Sandbox::RendererSource::SetCamera(const std::shared_ptr<Camera>& inCamera) { camera = inCamera; }
void Sandbox::RendererSource::BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments, VkExtent2D resolution)
{
    commandBuffer->TransitionImageLayout(renderAttachments->images[2], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    // TODO:这段应该不能放在这里
    if (outputImage->extent.width != resolution.width || outputImage->extent.height != resolution.height)
    {
        outputImageView != nullptr ? outputImageView->Cleanup() : void();
        outputImage != nullptr ? outputImage->Cleanup() : void();
        outputImage     = std::make_shared<Image>(commandBuffer->GetDevice(), VkExtent3D{resolution.width, resolution.height, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                              VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 1);
        outputImageView = std::make_shared<ImageView>(outputImage, VK_IMAGE_VIEW_TYPE_2D);
    }
    commandBuffer->TransitionImageLayout(outputImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    commandBuffer->BlitImage(renderAttachments->images[2], outputImage, resolution);
    commandBuffer->TransitionImageLayout(outputImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Sandbox::RendererSource::OnRecreateSwapchain(){
    
}
