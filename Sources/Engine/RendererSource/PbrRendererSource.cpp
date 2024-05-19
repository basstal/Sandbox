#include "pch.hpp"

#include "PbrRendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/Light.hpp"
#include "Engine/PostProcess.hpp"
#include "FileSystem/Directory.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"
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
    // if (camera != nullptr)
    // {
    //     camera->postProcess->Cleanup();
    // }

    // for (size_t i = 0; i < descriptorSets.size(); ++i)
    // {
    //     descriptorSets[i]->Cleanup();
    // }
    for (auto& uboLight : uboLights)
    {
        uboLight->Cleanup();
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
    // std::vector<std::set<uint32_t>> presets              = {{0, 1}, {2, 3}};
    // pipelineState->descriptorSetsPreset                  = std::make_shared<DescriptorSetsPreset>(presets);
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
    // descriptorSets.resize(frameFlightSize);
    uboLights.resize(frameFlightSize);
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        uboLights[i] = std::make_shared<UniformBuffer>(device, sizeof(Light));
        // descriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet(pipeline->pipelineLayout->descriptorSetLayouts[0], i);
    }
    // UpdateDescriptorSets(renderer);
}

void Sandbox::PbrRendererSource::UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer, std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets)
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
        descriptorSets[i]->BindBufferInfoMapping(bufferInfoMapping, pipeline->pipelineLayout->descriptorSetLayouts[0]);
    }
    onUpdateDescriptorSets.Trigger(shared_from_this());
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


