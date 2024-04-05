#include "pch.hpp"

#include "PbrRendererSource.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/Image.hpp"
#include "Engine/Light.hpp"
#include "FileSystem/Directory.hpp"
#include "Misc/TypeCasting.hpp"
#include "VulkanRHI/Common/Macros.hpp"
#include "VulkanRHI/Common/ShaderSource.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/ShaderModule.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/Texture.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

void Sandbox::PbrRendererSource::Prepare(std::shared_ptr<Sandbox::Renderer>& renderer) { RendererSource::Prepare(renderer); }

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
    auto device = renderer->device;

    Directory assetsDirectory = Directory::GetAssetsDirectory();
    auto      vertexSource    = ShaderSource(assetsDirectory.GetFile("Shaders/PBR.vert").path.string());
    auto      fragmentSource  = ShaderSource(assetsDirectory.GetFile("Shaders/PBR.frag").path.string());
    auto      vertexShader    = std::make_shared<ShaderModule>(device, vertexSource, preamble, VK_SHADER_STAGE_VERTEX_BIT);
    shaderModules.push_back(vertexShader);
    auto fragmentShader = std::make_shared<ShaderModule>(device, fragmentSource, preamble, VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderModules.push_back(fragmentShader);
    pipelineLayout = std::make_shared<PipelineLayout>(device, shaderModules, std::vector<uint32_t>{1});
    pipeline       = std::make_shared<Pipeline>(device, shaderModules, renderer->renderPass, pipelineLayout, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL);
}
void Sandbox::PbrRendererSource::CreateDescriptorSets(std::shared_ptr<Renderer>& renderer)
{
    auto device = renderer->device;

    uint32_t frameFlightSize = renderer->maxFramesFlight;
    descriptorSets.resize(frameFlightSize);
    uboLights.resize(frameFlightSize);
    textures.resize(frameFlightSize);

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
    uint32_t dynamicAlignment = renderer->GetUniformDynamicAlignment(sizeof(glm::mat4));
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        uboLights[i] = std::make_shared<UniformBuffer>(device, sizeof(Light));

        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uboMvp[i]->vpUbo->GetDescriptorBufferInfo()}},
            {1, {uboMvp[i]->modelsUbo->GetDescriptorBufferInfo(dynamicAlignment)}},
            {3, {uboLights[i]->GetDescriptorBufferInfo()}},
        };

        BindingMap<VkDescriptorImageInfo> imageInfoMapping = {
            {2,
             {textures[i][0]->GetDescriptorImageInfo(), textures[i][1]->GetDescriptorImageInfo(), textures[i][2]->GetDescriptorImageInfo(),
              textures[i][3]->GetDescriptorImageInfo()}},
        };
        descriptorSets[i] = std::make_shared<DescriptorSet>(device, renderer->descriptorPool, pipelineLayout->descriptorSetLayout, bufferInfoMapping, imageInfoMapping);
    }
}
