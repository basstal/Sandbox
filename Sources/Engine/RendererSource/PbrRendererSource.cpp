#include "pch.hpp"

#include "PbrRendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/Image.hpp"
#include "Engine/Light.hpp"
#include "Engine/PostProcess.hpp"
#include "FileSystem/Directory.hpp"
#include "Misc/Debug.hpp"
#include "Misc/TypeCasting.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Common/Caching/ShaderModuleCaching.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"
#include "VulkanRHI/Rendering/Texture.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"


void Sandbox::PbrRendererSource::UpdateUniforms(uint32_t frameFlightIndex)
{
    RendererSource::UpdateUniforms(frameFlightIndex);
    // static Light
    Light light;
    light.position = glm::vec3(0.0, 0.0, 1.0f);
    light.color    = glm::vec3(1.0f, 1.0f, 1.0f);
    uboLights[frameFlightIndex]->Update(&light);
}
void Sandbox::PbrRendererSource::Cleanup()
{
    if (camera != nullptr)
    {
        camera->postProcess->Cleanup();
    }
    for (size_t i = 0; i < descriptorSets.size(); ++i)
    {
        descriptorSets[i]->Cleanup();
        for (size_t t = 0; t < 4; ++t)
        {
            textures[i][t]->Cleanup();
        }
        uboLights[i]->Cleanup();
    }
    RendererSource::Cleanup();
}
void Sandbox::PbrRendererSource::CreatePipeline(std::shared_ptr<Renderer>& renderer) { CreatePipelineWithPreamble(renderer, ""); }

void Sandbox::PbrRendererSource::CreatePipelineWithPreamble(std::shared_ptr<Renderer>& renderer, const std::string& preamble)
{
    auto device               = renderer->device;
    shaderLinkage             = std::make_shared<ShaderLinkage>();
    Directory assetsDirectory = Directory::GetAssetsDirectory();
    auto      vertexSource    = std::make_shared<ShaderSource>(assetsDirectory.GetFile("Shaders/PBR.vert").path.string(), preamble);
    auto      vertexShader    = shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
    vertexShader->SetUniformDescriptorMode("Model", Dynamic);

    auto fragmentSource = std::make_shared<ShaderSource>(assetsDirectory.GetFile("Shaders/PBR.frag").path.string(), preamble);

    auto fragmentShader = shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);


    // pipelineLayout = std::make_shared<PipelineLayout>(device, shaderModules);

    pipelineState                                        = std::make_shared<PipelineState>(shaderLinkage, renderer->renderPass);
    pipelineState->multisampleState.rasterizationSamples = device->GetMaxUsableSampleCount();
    pipeline                                             = renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);
    auto pipelineLayout                                  = pipeline->pipelineLayout;
    // TODO:这里只处理了 pushConstants 唯一的情况
    pushConstantsInfo.size  = pipelineLayout->pushConstantRanges[0].size;
    pushConstantsInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    // auto anotherPipeline = renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);
    // LOGD("Debug", "{}, {}", PtrToHexString(pipeline.get()), PtrToHexString(anotherPipeline.get()))
}
void Sandbox::PbrRendererSource::CreateDescriptorSets(std::shared_ptr<Renderer>& renderer)
{
    auto device = renderer->device;

    uint32_t frameFlightSize = renderer->maxFramesFlight;
    descriptorSets.resize(frameFlightSize);
    uboLights.resize(frameFlightSize);
    textures.resize(frameFlightSize);

    // TODO:简化以下贴图载入
    std::shared_ptr<Sandbox::Resource::Image> albedo    = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/albedo.png"));
    std::shared_ptr<Sandbox::Resource::Image> metallic  = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/metallic.png"));
    std::shared_ptr<Sandbox::Resource::Image> roughness = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/roughness.png"));
    std::shared_ptr<Sandbox::Resource::Image> ao        = std::make_shared<Resource::Image>(Directory::GetAssetsDirectory().GetFile("Textures/pbr/rusted_iron/ao.png"));

    auto albedoImage    = std::make_shared<Image>(device, VkExtent3D{ToUInt32(albedo->width), ToUInt32(albedo->height), 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                               VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                               VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, albedo->mipLevels);
    auto metallicImage  = std::make_shared<Image>(device, VkExtent3D{ToUInt32(metallic->width), ToUInt32(metallic->height), 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                                 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                                 VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, metallic->mipLevels);
    auto roughnessImage = std::make_shared<Image>(device, VkExtent3D{ToUInt32(roughness->width), ToUInt32(roughness->height), 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                                  VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, roughness->mipLevels);
    auto aoImage        = std::make_shared<Image>(device, VkExtent3D{ToUInt32(ao->width), ToUInt32(ao->height), 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                           VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                           VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, ao->mipLevels);

    renderer->commandBuffers[0]->CopyDataToImage(albedo, albedoImage, VK_FORMAT_R8G8B8A8_UNORM);
    renderer->commandBuffers[0]->CopyDataToImage(metallic, metallicImage, VK_FORMAT_R8G8B8A8_UNORM);
    renderer->commandBuffers[0]->CopyDataToImage(roughness, roughnessImage, VK_FORMAT_R8G8B8A8_UNORM);
    renderer->commandBuffers[0]->CopyDataToImage(ao, aoImage, VK_FORMAT_R8G8B8A8_UNORM);

    for (size_t i = 0; i < textures.size(); ++i)
    {
        textures[i][0]            = std::make_shared<Texture>(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][0]->image     = albedoImage;
        textures[i][0]->imageView = std::make_shared<ImageView>(albedoImage, VK_IMAGE_VIEW_TYPE_2D);
        textures[i][1]            = std::make_shared<Texture>(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][1]->image     = metallicImage;
        textures[i][1]->imageView = std::make_shared<ImageView>(metallicImage, VK_IMAGE_VIEW_TYPE_2D);
        textures[i][2]            = std::make_shared<Texture>(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][2]->image     = roughnessImage;
        textures[i][2]->imageView = std::make_shared<ImageView>(roughnessImage, VK_IMAGE_VIEW_TYPE_2D);
        textures[i][3]            = std::make_shared<Texture>(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        textures[i][3]->image     = aoImage;
        textures[i][3]->imageView = std::make_shared<ImageView>(aoImage, VK_IMAGE_VIEW_TYPE_2D);
    }
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        uboLights[i]      = std::make_shared<UniformBuffer>(device, sizeof(Light));
        descriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet(pipeline->pipelineLayout->descriptorSetLayout, i);
    }
    UpdateDescriptorSets(renderer);
}

void Sandbox::PbrRendererSource::UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer)
{
    uint32_t frameFlightSize  = renderer->maxFramesFlight;
    uint32_t dynamicAlignment = renderer->GetUniformDynamicAlignment(sizeof(glm::mat4));

    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uboMvp[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uboMvp[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
            {3, {uboLights[i]->GetDescriptorBufferInfo()}},
        };
        descriptorSets[i]->BindBufferInfoMapping(bufferInfoMapping, pipeline->pipelineLayout->descriptorSetLayout);
        BindingMap<VkDescriptorImageInfo> imageInfoMapping = {
            {2,
             {textures[i][0]->GetDescriptorImageInfo(), textures[i][1]->GetDescriptorImageInfo(), textures[i][2]->GetDescriptorImageInfo(),
              textures[i][3]->GetDescriptorImageInfo()}},
        };
        descriptorSets[i]->BindImageInfoMapping(imageInfoMapping, pipeline->pipelineLayout->descriptorSetLayout);
    }
}
void Sandbox::PbrRendererSource::PushConstants(const std::shared_ptr<CommandBuffer>& inCommandBuffer)
{
    RendererSource::PushConstants(inCommandBuffer);
    inCommandBuffer->PushConstants(pipeline->pipelineLayout, pushConstantsInfo);
}
void Sandbox::PbrRendererSource::BindPipeline(const std::shared_ptr<CommandBuffer>& inCommandBuffer)
{
    RendererSource::BindPipeline(inCommandBuffer);
    inCommandBuffer->BindPipeline(pipeline);
}
void Sandbox::PbrRendererSource::BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments,
                                           VkExtent2D resolution)
{
    RendererSource::BlitImage(commandBuffer, renderAttachments, resolution);
    if (camera != nullptr)
    {
        // for (auto& postProcess : camera->postProcess)
        // {
        // if (camera->postProcess->IsPrepared())
        // {
        camera->postProcess->Apply(commandBuffer, renderAttachments, resolution, shared_from_this());
        // RendererSource::BlitImage(commandBuffer, renderAttachments, resolution);
        // }
        // }
        // if (!postProcesses.empty())
        // {
        // }
    }
}
void Sandbox::PbrRendererSource::Tick(const std::shared_ptr<Renderer>& renderer)
{
    RendererSource::Tick(renderer);
    if (camera != nullptr)
    {
        if (!camera->postProcess->IsPrepared())
        {
            camera->postProcess->Prepare(renderer);
        }
        // for (auto& [_, postProcess] : camera->postProcessFragShaders)
        // {
        //     if (postProcess != nullptr && !postProcess->IsPrepared())
        //     {
        //         postProcess->Prepare(renderer);
        //         postProcesses.push_back(postProcess);
        //     }
        // }
        // for (int i = ToInt32(postProcesses.size()) - 1; i >= 0; --i)
        // {
        //     if (!postProcesses[i]->IsPrepared())
        //     {
        //         postProcesses.erase(postProcesses.begin() + i);
        //     }
        // }
    }
}

// void Sandbox::PbrRendererSource::BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments,
//                                            VkExtent2D resolution)
// {
//     RendererSource::BlitImage(commandBuffer, renderAttachments, resolution);
//
// }
// void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image,
//                        VkImageLayout oldLayout, VkImageLayout newLayout,
//                        VkImageSubresourceRange subresourceRange) {
//     VkImageMemoryBarrier barrier{};
//     barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//     barrier.oldLayout = oldLayout;
//     barrier.newLayout = newLayout;
//     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//     barrier.image = image;
//     barrier.subresourceRange = subresourceRange;
//
//     VkPipelineStageFlags sourceStage;
//     VkPipelineStageFlags destinationStage;
//
//     if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
//         barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//         barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//         sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//         destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     } else {
//         // Handle other transitions
//     }
//
//     vkCmdPipelineBarrier(commandBuffer,
//                          sourceStage, destinationStage,
//                          0, 0, nullptr, 0, nullptr,
//                          1, &barrier);
    // }


