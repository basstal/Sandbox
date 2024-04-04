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
    pipelineLayout = std::make_shared<PipelineLayout>(device, shaderModules, std::vector<uint32_t>{1});
    pipeline       = std::make_shared<Pipeline>(device, shaderModules, renderer->renderPass, pipelineLayout, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE);
}
void Sandbox::WireframeRendererSource::CreateDescriptorSets(std::shared_ptr<Renderer>& renderer)
{
    auto     device          = renderer->device;
    uint32_t frameFlightSize = renderer->maxFramesFlight;
    descriptorSets.resize(frameFlightSize);

    uint32_t dynamicAlignment = renderer->GetUniformDynamicAlignment(sizeof(glm::mat4));
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uboMvp[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uboMvp[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
        };
        BindingMap<VkDescriptorImageInfo> imageInfoMapping;
        descriptorSets[i] = std::make_shared<DescriptorSet>(device, renderer->descriptorPool, pipelineLayout->descriptorSetLayout, bufferInfoMapping, imageInfoMapping);
    }
}
