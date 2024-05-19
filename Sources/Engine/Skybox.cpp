#include "pch.hpp"

#include "Skybox.hpp"

#include "Editor/Grid.hpp"
#include "Engine.hpp"
#include "EntityComponent/Components/Camera.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/Logger.hpp"
#include "Images/Image.hpp"
#include "Misc/TypeCasting.hpp"
#include "RendererSource/RendererSource.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/Sampler.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"
#include "VulkanRHI/Rendering/UniformBuffer.hpp"

std::shared_ptr<Sandbox::Buffer> Sandbox::Skybox::CreateVertexBuffer(const std::shared_ptr<Renderer>& renderer)
{
    const float cubeVertices[] = {// positions
                                  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,  // -z

                                  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  // -x

                                  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,  // +x

                                  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  // +z

                                  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,  // +y

                                  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};  // -y

    auto         commandBuffer = renderer->commandBuffers[0];
    VkDeviceSize bufferSize    = sizeof(float) * 3 * 36;
    auto         buffer =
        std::make_shared<Buffer>(renderer->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    commandBuffer->CopyDataToBuffer(cubeVertices, bufferSize, buffer);
    return buffer;
}

Sandbox::Skybox:: Skybox() : m_cleaned(true) {}
Sandbox::Skybox::~Skybox() { Cleanup(); }

void Sandbox::Skybox::Prepare(const std::shared_ptr<Renderer>& renderer)
{
    m_renderer         = renderer;
    m_vertexBuffer     = CreateVertexBuffer(renderer);
    auto shaderLinkage = std::make_shared<ShaderLinkage>();
    shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_VERTEX_BIT, std::make_shared<ShaderSource>(Directory::GetAssetsDirectory().GetFile("Shaders/Cubemaps.vert"), ""));
    shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_FRAGMENT_BIT,
                                    std::make_shared<ShaderSource>(Directory::GetAssetsDirectory().GetFile("Shaders/Cubemaps.frag"), ""));
    auto pipelineState                                   = std::make_shared<PipelineState>(shaderLinkage, renderer->renderPass);
    pipelineState->multisampleState.rasterizationSamples = m_renderer->device->GetMaxUsableSampleCount();
    pipelineState->depthStencilState.depthTestEnable     = VK_FALSE;
    pipelineState->depthStencilState.depthWriteEnable    = VK_FALSE;
    // pipelineState->rasterizationState.cullMode           = VK_CULL_MODE_FRONT_BIT;
    m_pipeline = renderer->pipelineCaching->GetOrCreatePipeline(pipelineState);
    m_descriptorSets.resize(renderer->maxFramesFlight);
    for (size_t i = 0; i < m_descriptorSets.size(); ++i)
    {
        m_descriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet("default", m_pipeline->pipelineLayout->descriptorSetLayouts[0], i);
    }
    m_sampler                  = std::make_shared<Sampler>(m_renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    m_cleaned                  = false;
    m_beforeRendererDrawHandle = renderer->onBeforeRendererDraw.BindMember<Skybox, &Skybox::Draw>(this, 1);
}

bool Sandbox::Skybox::IsPrepared() { return m_cleaned == false; }

void Sandbox::Skybox::CreateCubemapImages(const std::vector<std::shared_ptr<Resource::Image>>& skyboxImages)
{
    auto skyboxImage   = skyboxImages[0];
    auto extent3d      = skyboxImage->GetExtent3D();
    m_image            = std::make_shared<Image>(m_renderer->device, extent3d, VK_FORMAT_R8G8B8A8_UNORM,
                                      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                      VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 1, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    m_imageView        = std::make_shared<ImageView>(m_image, VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R8G8B8A8_UNORM);
    auto commandBuffer = m_renderer->commandBuffers[0];
    commandBuffer->CopyDataToImageMultiLayers(skyboxImages, m_image, VK_FORMAT_R8G8B8A8_UNORM);
    commandBuffer->TransitionImageLayoutInstant(m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 6);


    std::shared_ptr<RendererSource> rendererSource;
    auto                            viewMode = m_renderer->viewMode;
    if (!m_renderer->TryGetRendererSource(viewMode, rendererSource))
    {
        LOGF("VulkanRHI", "Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[ToUInt32(viewMode)])
    }
    auto uniformMvpObjectsNoMoving = rendererSource->uboMvpNoMoving;

    for (size_t i = 0; i < m_descriptorSets.size(); ++i)
    {
        // TODO:这里切了 rendererSource 以后应该不对
        // TODO:这里需要自己的 vpUbo 并且需要做裁剪平移 glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        BindingMap<VkDescriptorBufferInfo> bufferInfoMapping = {
            {0, {uniformMvpObjectsNoMoving[i]->vpUbo->GetDescriptorBufferInfo()}},
        };
        m_descriptorSets[i]->BindBufferInfoMapping(bufferInfoMapping, m_pipeline->pipelineLayout->descriptorSetLayouts[0]);

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout                                                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView                                                       = m_imageView->vkImageView;
        imageInfo.sampler                                                         = m_sampler->vkSampler;
        std::map<uint32_t, std::vector<VkDescriptorImageInfo>> inImageInfoMapping = {{1, {imageInfo}}};
        m_descriptorSets[i]->BindImageInfoMapping(inImageInfoMapping, m_pipeline->pipelineLayout->descriptorSetLayouts[0]);
    }
}

void Sandbox::Skybox::Draw(const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t frameFlightIndex)
{
    commandBuffer->BindPipeline(m_pipeline);
    commandBuffer->BindDescriptorSet(m_pipeline->pipelineLayout, m_descriptorSets[frameFlightIndex]);
    commandBuffer->BindVertexBuffers(m_vertexBuffer);
    commandBuffer->Draw(36);
}
void Sandbox::Skybox::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    m_renderer->onBeforeRendererDraw.Unbind(m_beforeRendererDrawHandle);
    m_imageView != nullptr ? m_imageView->Cleanup() : void();
    m_image != nullptr ? m_image->Cleanup() : void();
    m_sampler->Cleanup();
    m_vertexBuffer->Cleanup();
    
}
