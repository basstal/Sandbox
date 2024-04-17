#include "pch.hpp"

#include "WireframeRendererSource.hpp"

#include "FileSystem/Directory.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "VulkanRHI/Common/PipelineCaching.hpp"
#include "VulkanRHI/Common/ShaderModuleCaching.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"
void Sandbox::WireframeRendererSource::CreatePipeline(std::shared_ptr<Renderer>& renderer)
{
    auto device = renderer->device;

    Directory assetsDirectory = Directory::GetAssetsDirectory();
    auto      vertexSource    = ShaderSource(assetsDirectory.GetFile("Shaders/FillModeNonSolid.vert").path.string(), "", VK_SHADER_STAGE_VERTEX_BIT);
    auto      vertexShader    = renderer->shaderModuleCaching->GetOrCreateShaderModule(vertexSource);
    vertexShader->SetUniformDescriptorMode("Model", Dynamic);
    shaderModules.push_back(vertexShader);

    auto fragmentSource = ShaderSource(assetsDirectory.GetFile("Shaders/FillModeNonSolid.frag").path.string(), "", VK_SHADER_STAGE_FRAGMENT_BIT);
    auto fragmentShader = renderer->shaderModuleCaching->GetOrCreateShaderModule(fragmentSource);
    shaderModules.push_back(fragmentShader);
    // pipelineLayout                                = std::make_shared<PipelineLayout>(device, shaderModules);
    pipelineState                                 = std::make_shared<PipelineState>(shaderModules, renderer->renderPass);
    pipelineState->rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
    pipeline                                      = renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);
    
    colorUniformBuffer                            = std::make_shared<UniformBuffer>(device, sizeof(glm::vec3));
    auto white                                    = glm::vec3(1.0f);
    colorUniformBuffer->Update(&white);
}
void Sandbox::WireframeRendererSource::CreateDescriptorSets(std::shared_ptr<Renderer>& renderer)
{
    auto     device          = renderer->device;
    uint32_t frameFlightSize = renderer->maxFramesFlight;
    descriptorSets.resize(frameFlightSize);
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        descriptorSets[i] = std::make_shared<DescriptorSet>(device, renderer->descriptorPool, pipeline->pipelineLayout->descriptorSetLayout);
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
        BindingMap<VkDescriptorImageInfo> imageInfoMapping;
        descriptorSets[i]->BindInfoMapping(bufferInfoMapping, imageInfoMapping, pipeline->pipelineLayout->descriptorSetLayout);
    }
}
void Sandbox::WireframeRendererSource::Cleanup()
{
    colorUniformBuffer != nullptr ? colorUniformBuffer->Cleanup() : void();
    RendererSource::Cleanup();
}
