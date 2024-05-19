#include "pch.hpp"

#include "Material.hpp"

#include "Engine/EntityComponent/GameObject.hpp"
#include "Engine/Images/Image.hpp"
#include "Engine/RendererSource/PbrRendererSource.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "Engine/RendererSource/WireframeRendererSource.hpp"
#include "FileSystem/Directory.hpp"
#include "Generated/Material.rfks.h"
#include "Mesh.hpp"
#include "Misc/TypeCasting.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/Texture.hpp"

static uint32_t    instanceId = 0;
Sandbox::Material::Material()
{
    onComponentCreate.Bind(
        [this](const std::shared_ptr<IComponent>& inComponent)
        {
            if (inComponent.get() == this)
            {
                m_mesh = gameObject.lock()->GetComponent<Mesh>();
            }
        });
    m_cleaned    = true;
    m_instanceId = instanceId++;
}

/**
 * 这里只对应到 lit 模式
 * @param renderer
 */
void Sandbox::Material::Prepare(const std::shared_ptr<Renderer>& renderer)
{
    if (!m_cleaned)
    {
        return;
    }
    m_cleaned                = false;
    m_renderer               = renderer;
    uint32_t frameFlightSize = renderer->maxFramesFlight;
    descriptorSets.resize(frameFlightSize);
    auto pbrRendererSource = std::dynamic_pointer_cast<PbrRendererSource>(renderer->rendererSourceMapping[EViewMode::Lit]);
    auto pipeline          = pbrRendererSource->pipeline;
    for (size_t i = 0; i < frameFlightSize; ++i)
    {
        descriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet(std::format("mat_{}", m_instanceId), pipeline->pipelineLayout->descriptorSetLayouts[0], i);
    }
    LoadImages(renderer);
}

void Sandbox::Material::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    IComponent::Cleanup();
    for (const auto& descriptorSet : descriptorSets)
    {
        descriptorSet->Cleanup();
    }
    descriptorSets.clear();
    for (size_t t = 0; t < 4; ++t)
    {
        if (textures[t] != nullptr)
        {
            textures[t]->Cleanup();
        }
    }
}

std::shared_ptr<Sandbox::DescriptorSet> Sandbox::Material::GetDescriptorSet(uint32_t frameFlightIndex, const std::shared_ptr<RendererSource>& rendererSource)
{
    auto wireframeRendererSource = std::dynamic_pointer_cast<WireframeRendererSource>(rendererSource);
    if (wireframeRendererSource != nullptr)
    {
        return wireframeRendererSource->descriptorSets[frameFlightIndex];
    }
    return descriptorSets[frameFlightIndex];
}
void Sandbox::Material::DrawMesh(const std::shared_ptr<Renderer>& inRenderer, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                                 const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets)
{
    if (m_cleaned)
    {
        // LOGW("Game", "Material has been cleaned, but still using it in mesh draw!")
        return;  // 如果已被清理，则跳过绘制
    }
    // TODO:临时写的，这里应该还需要再设计一下
    if (customRendererSource == nullptr)
    {
        // TODO:这里应该需要确保 pipelineState 没有中途变更
        auto pipeline = inRenderer->pipelineCaching->GetOrCreatePipeline(rendererSource->pipelineState);
        // TODO: pipeline 相同不需要 bind，需要合批
        commandBuffer->BindPipeline(pipeline);
        rendererSource->PushConstants(commandBuffer);

        auto descriptorSet = GetDescriptorSet(frameFlightIndex, rendererSource);
        // commandBuffer->PushConstants(pushConstantsInfo);
        commandBuffer->BindDescriptorSet(pipeline->pipelineLayout, descriptorSet, {dynamicOffsets});
        commandBuffer->BindVertexBuffers(m_mesh->vertexBuffer);
        commandBuffer->BindIndexBuffer(m_mesh->indexBuffer);
        commandBuffer->DrawIndexed(m_mesh->Indices());
    }
    else
    {
        customRendererSource->CustomDrawMesh(m_mesh, commandBuffer, descriptorSets[frameFlightIndex], frameFlightIndex, dynamicOffsets);
    }
}

void Sandbox::Material::DrawOverlay(const std::shared_ptr<Renderer>& inRenderer, const std::shared_ptr<RendererSource>& rendererSource, uint32_t frameFlightIndex,
                                    const std::shared_ptr<CommandBuffer>& commandBuffer, uint32_t dynamicOffsets)
{
    if (m_cleaned)
    {
        // LOGW("Game", "Material has been cleaned, but still using it in mesh draw!")
        return;  // 如果已被清理，则跳过绘制
    }
    if (customRendererSource == nullptr)
    {
        // pass
    }
    else
    {
        customRendererSource->CustomDrawOverlay(m_mesh, commandBuffer, descriptorSets[frameFlightIndex], frameFlightIndex, dynamicOffsets);
    }
}


void LoadImage(std::shared_ptr<Sandbox::Image>& image, const Sandbox::String& inPath, const std::shared_ptr<Sandbox::Renderer>& renderer)
{
    auto imageFile = Sandbox::Directory::GetAssetsDirectory().GetFile(inPath.ToStdString());

    auto     device      = renderer->device;
    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;  // 图像格式

    if (imageFile.Exists())
    {
        auto imageRaw = Sandbox::Resource::Image::Load(imageFile, Sandbox::Resource::Color);
        image         = std::make_shared<Sandbox::Image>(device, VkExtent3D{Sandbox::ToUInt32(imageRaw->width), Sandbox::ToUInt32(imageRaw->height), 1}, imageFormat,
                                                 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                                                 VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, imageRaw->mipLevels);

        renderer->commandBuffers[0]->CopyDataToImage(image, imageRaw, imageFormat);
    }
    else
    {
        VkExtent3D imageExtent = {16, 16, 1};  // 默认图像尺寸

        // 计算需要的像素数量
        size_t               numPixels = Sandbox::ToInt32(imageExtent.width * imageExtent.height);
        std::vector<uint8_t> allPixels(numPixels * 4, 255);  // 填充为 RGBA = 255, 255, 255, 255
        image =
            std::make_shared<Sandbox::Image>(device, imageExtent, imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                             VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 1);  // 使用 1 个 mip 级别

        VkExtent2D extent2D = {imageExtent.width, imageExtent.height};
        // 上传像素数据到图像
        renderer->commandBuffers[0]->CopyDataToImage(image, allPixels.data(), extent2D, imageFormat, 1);
    }
}
void Sandbox::Material::LoadImages(const std::shared_ptr<Renderer>& renderer)
{
    if (m_cleaned)
    {
        return;
    }
    LoadImage(albedoImage, albedoPath, renderer);
    LoadImage(metallicImage, metallicPath, renderer);
    LoadImage(roughnessImage, roughnessPath, renderer);
    LoadImage(aoImage, aoPath, renderer);
    auto device        = renderer->device;
    auto assignTexture = [device](std::shared_ptr<Texture>& inTexture, std::shared_ptr<Image>& inImage)
    {
        if (inTexture != nullptr)
        {
            inTexture->Cleanup();
        }
        inTexture = std::make_shared<Texture>(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        if (inImage != nullptr)
        {
            inTexture->image     = inImage;
            inTexture->imageView = std::make_shared<ImageView>(inImage, VK_IMAGE_VIEW_TYPE_2D);
        }
    };
    assignTexture(textures[0], albedoImage);
    assignTexture(textures[1], metallicImage);
    assignTexture(textures[2], roughnessImage);
    assignTexture(textures[3], aoImage);
    auto pbrRendererSource = std::dynamic_pointer_cast<PbrRendererSource>(m_renderer->rendererSourceMapping[EViewMode::Lit]);
    UpdateDescriptorSets(pbrRendererSource);
}


void Sandbox::Material::UpdateDescriptorSets(const std::shared_ptr<PbrRendererSource>& rendererSource)
{
    // auto descriptorSets = rendererSource->descriptorSets;
    auto pipeline = m_renderer->pipelineCaching->GetOrCreatePipeline(rendererSource->pipelineState);
    for (size_t i = 0; i < descriptorSets.size(); ++i)
    {
        BindingMap<VkDescriptorImageInfo> imageInfoMapping = {
            {2, {textures[0]->GetDescriptorImageInfo(), textures[1]->GetDescriptorImageInfo(), textures[2]->GetDescriptorImageInfo(), textures[3]->GetDescriptorImageInfo()}},
        };
        descriptorSets[i]->BindImageInfoMapping(imageInfoMapping, pipeline->pipelineLayout->descriptorSetLayouts[0]);
    }
    rendererSource->UpdateDescriptorSets(m_renderer, descriptorSets);
}
