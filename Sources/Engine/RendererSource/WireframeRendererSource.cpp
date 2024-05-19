#include "pch.hpp"

#include "WireframeRendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Common/Caching/ShaderModuleCaching.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"
void Sandbox::WireframeRendererSource::CreatePipeline(std::shared_ptr<Renderer>& renderer)
{
    auto device = renderer->device;

    Directory assetsDirectory = Directory::GetAssetsDirectory();
    shaderLinkage = std::make_shared<ShaderLinkage>();
    auto vertexSource = std::make_shared<ShaderSource>(assetsDirectory.GetFile("Shaders/FillModeNonSolid.vert").path.string(), "");
    auto vertexShader = shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
    vertexShader->SetUniformDescriptorMode("Model", Dynamic);

    auto fragmentSource = std::make_shared<ShaderSource>(assetsDirectory.GetFile("Shaders/FillModeNonSolid.frag").path.string(), "");
    auto fragmentShader = shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);
    // pipelineLayout                                = std::make_shared<PipelineLayout>(device, shaderModules);
    pipelineState                                 = std::make_shared<PipelineState>(shaderLinkage, renderer->renderPass);
    pipelineState->multisampleState.rasterizationSamples = renderer->device->GetMaxUsableSampleCount();
    pipelineState->rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
    pipeline                                      = renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);

    colorUniformBuffer = std::make_shared<UniformBuffer>(device, sizeof(glm::vec3));
    auto white         = glm::vec3(1.0f);
    colorUniformBuffer->Update(&white);
}
void Sandbox::WireframeRendererSource::CreateDescriptorSets(std::shared_ptr<Renderer>& renderer)
{
    auto     device          = renderer->device;
    uint32_t frameFlightSize = renderer->maxFramesFlight;
    descriptorSets.resize(frameFlightSize);
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        descriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet("default", pipeline->pipelineLayout->descriptorSetLayouts[0], i);
    }
    UpdateDescriptorSets(renderer);
}

void Sandbox::WireframeRendererSource::UpdateDescriptorSets(const std::shared_ptr<Renderer>& renderer)
{
    uint32_t frameFlightSize  = renderer->maxFramesFlight;
    uint32_t dynamicAlignment = renderer->GetUniformDynamicAlignment(sizeof(glm::mat4));

    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uboMvp[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uboMvp[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
            {2, {colorUniformBuffer->GetDescriptorBufferInfo()}},
        };
        descriptorSets[i]->BindBufferInfoMapping(bufferInfoMapping, pipeline->pipelineLayout->descriptorSetLayouts[0]);
    }
}
void Sandbox::WireframeRendererSource::Cleanup()
{
    colorUniformBuffer != nullptr ? colorUniformBuffer->Cleanup() : void();
    RendererSource::Cleanup();
}
void Sandbox::WireframeRendererSource::BindPipeline(const std::shared_ptr<CommandBuffer>& inCommandBuffer)
{
    RendererSource::BindPipeline(inCommandBuffer);
    inCommandBuffer->BindPipeline(pipeline);
}
