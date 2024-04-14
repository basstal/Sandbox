#include "pch.hpp"

#include "WireframeRendererSource.hpp"

#include "FileSystem/Directory.hpp"
#include "VulkanRHI/Common/Macros.hpp"
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
    auto      vertexSource    = ShaderSource(assetsDirectory.GetFile("Shaders/FillModeNonSolid.vert").path.string());
    auto      fragmentSource  = ShaderSource(assetsDirectory.GetFile("Shaders/FillModeNonSolid.frag").path.string());
    auto      vertexShader    = std::make_shared<ShaderModule>(device, vertexSource, "", VK_SHADER_STAGE_VERTEX_BIT);
    shaderModules.push_back(vertexShader);
    auto fragmentShader = std::make_shared<ShaderModule>(device, fragmentSource, "", VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderModules.push_back(fragmentShader);
    pipelineLayout                                = std::make_shared<PipelineLayout>(device, shaderModules, std::vector<uint32_t>{1});
    auto pipelineState                            = std::make_shared<PipelineState>(shaderModules, renderer->renderPass, pipelineLayout);
    pipelineState->rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
    pipeline                                      = std::make_shared<Pipeline>(device, pipelineState);
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
        descriptorSets[i] = std::make_shared<DescriptorSet>(device, renderer->descriptorPool, pipelineLayout->descriptorSetLayout);
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
        descriptorSets[i]->BindInfoMapping(bufferInfoMapping, imageInfoMapping, pipelineLayout->descriptorSetLayout);
    }
}
void Sandbox::WireframeRendererSource::Cleanup()
{
    colorUniformBuffer != nullptr ? colorUniformBuffer->Cleanup() : void();
    RendererSource::Cleanup();
}
