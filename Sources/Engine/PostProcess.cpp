#include "pch.hpp"

#include "PostProcess.hpp"

#include "BasicMeshData.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/File.hpp"
#include "FileSystem/Logger.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageMemoryBarrier.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Core/RenderPass.hpp"
#include "VulkanRHI/Core/Sampler.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/RenderAttachments.hpp"
#include "VulkanRHI/Rendering/RenderTarget.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"

Sandbox::PostProcess::PostProcess() { m_cleaned = true; }

Sandbox::PostProcess::PostProcess(const std::vector<std::shared_ptr<ShaderSource>>& shaderSource)
{
    // 复制 shaderSource 的内容到 postProcessShaderSource
    m_postProcessShaderSource = shaderSource;
    m_cleaned                 = true;
}

Sandbox::PostProcess::~PostProcess() { Cleanup(); }

void Sandbox::PostProcess::Prepare(const std::shared_ptr<Renderer>& renderer)
{
    m_renderer           = renderer;
    m_cleaned            = false;
    auto postProcessSize = m_postProcessShaderSource.size();

    auto imageSize = 2;
    if (renderpassImages.empty())
    {
        renderpassImages.resize(imageSize);
        renderpassImageViews.resize(imageSize);
        for (size_t i = 0; i < renderpassImages.size(); ++i)
        {
            LOGD("VulkanRHI", "index : {}", std::to_string(i))
            renderpassImages[i] =
                std::make_shared<Image>(m_renderer->device, VkExtent3D{m_renderer->resolution.width, m_renderer->resolution.height, 1}, VK_FORMAT_R8G8B8A8_UNORM,
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
            renderpassImageViews[i] = std::make_shared<ImageView>(renderpassImages[i], VK_IMAGE_VIEW_TYPE_2D);
            renderer->commandBuffers[0]->TransitionImageLayoutInstant(renderpassImages[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        }

        sampler = std::make_shared<Sampler>(m_renderer->device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    }

    if (postProcessSize > 0)
    {
        renderpasses.resize(postProcessSize);
        for (size_t i = 0; i < postProcessSize; ++i)
        {
            std::vector<Attachment> attachments = {
                Attachment{VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
            };
            std::vector<LoadStoreInfo> loadStoreInfos = {
                LoadStoreInfo{VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE},
            };

            std::vector<SubpassInfo> subpassInfos(1);
            subpassInfos[0].colorAttachments = {0};

            // auto renderpassDependency = RenderPass::CreateDefaultDependency();
            // renderpassDependency.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
            // renderpassDependency.dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            // renderpassDependency.srcAccessMask |=
            //     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            // renderpassDependency.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
            renderpasses[i] = std::make_shared<RenderPass>(m_renderer->device, attachments, loadStoreInfos, subpassInfos);
        }

        shaderLinkages.resize(postProcessSize);
        pipelineStates.resize(postProcessSize);
        pipelines.resize(postProcessSize);
        for (size_t i = 0; i < shaderLinkages.size(); ++i)
        {
            auto shaderLinkage      = std::make_shared<ShaderLinkage>();
            auto vertexShaderSource = std::make_shared<ShaderSource>(Directory::GetAssetsDirectory().GetFile("Shaders/BlitDepth.vert"), "");
            shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_VERTEX_BIT, vertexShaderSource);
            shaderLinkage->LinkShaderModule(renderer, VK_SHADER_STAGE_FRAGMENT_BIT, m_postProcessShaderSource[i]);
            shaderLinkages[i] = shaderLinkage;
            pipelineStates[i] = std::make_shared<PipelineState>(shaderLinkage, renderpasses[i]);
            pipelines[i]      = renderer->pipelineCaching->GetOrCreatePipeline(pipelineStates[i]);
        }
        descriptorSets.resize(postProcessSize);
        OnRecreateSwapchain();
        for (size_t i = 0; i < descriptorSets.size(); ++i)
        {
            descriptorSets[i].resize(m_renderer->maxFramesFlight);
            for (size_t j = 0; j < m_renderer->maxFramesFlight; ++j)
            {
                // NOTE:管线使用同样的资源描述符
                descriptorSets[i][j] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet(pipelines[i]->pipelineLayout->descriptorSetLayout, j);
            }
        }
    }
}

bool Sandbox::PostProcess::IsPrepared() { return m_cleaned == false; }
void Sandbox::PostProcess::Apply(const std::shared_ptr<Sandbox::CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& inRenderAttachments,
                                 VkExtent2D resolution, const std::shared_ptr<RendererSource>& rendererSource)
{
    auto postProcessSize = m_postProcessShaderSource.size();
    if (postProcessSize == 0)
    {
        return;
    }
    auto clearColor        = VkClearColorValue{{1.0f, 1.0f, 1.0f, 1.0f}};
    auto clearDepthStencil = VkClearDepthStencilValue{1.0f, 0};
    auto frameFlightIndex  = m_renderer->frameFlightIndex;
    // commandBuffer->TransitionImageLayout(inRenderAttachments->images[2], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    commandBuffer->BlitImage(inRenderAttachments->images[2], renderpassImages[1], resolution);
    ImageMemoryBarrier barrier{};
    barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    barrier.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    commandBuffer->TransitionImageLayout(renderpassImageViews[1], barrier, VK_DEPENDENCY_BY_REGION_BIT);
    size_t i = 0;
    for (; i < postProcessSize; ++i)
    {
        commandBuffer->BeginRenderPass(renderpasses[i], renderTargets[i][m_renderer->swapchain->acquiredNextImageIndex]->framebuffer, resolution, clearColor, clearDepthStencil);
        commandBuffer->BindPipeline(pipelines[i]);
        auto sampledImageIndex = (i + 1) % 2;
        {
            VkDescriptorImageInfo imageInfo;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = renderpassImageViews[sampledImageIndex]->vkImageView;
            imageInfo.sampler     = sampler->vkSampler;

            std::map<uint32_t, std::vector<VkDescriptorImageInfo>> imageInfoMapping{{0, {imageInfo}}};
            descriptorSets[i][frameFlightIndex]->BindImageInfoMapping(imageInfoMapping, pipelines[i]->pipelineLayout->descriptorSetLayout);
        }
        // NOTE:使用同样的资源描述符
        commandBuffer->BindDescriptorSet(pipelines[i]->pipelineLayout, descriptorSets[i][frameFlightIndex]);
        commandBuffer->BindVertexBuffers(BasicMeshData::Instance->fullScreenQuad);
        commandBuffer->Draw(BasicMeshData::Instance->fullScreenQuadPoints);
        commandBuffer->EndRenderPass();
        ImageMemoryBarrier barrier4{};
        barrier4.oldLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier4.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier4.srcStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        barrier4.dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        barrier4.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        barrier4.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        // LOGD("VulkanRHI", "image memory barrier for {}", std::to_string((sampledImageIndex + 1) % 2))
        commandBuffer->TransitionImageLayout(renderpassImageViews[(sampledImageIndex + 1) % 2], barrier4);
    }
    auto outImageIndex = (i + 1) % 2;

    ImageMemoryBarrier barrier1{};
    barrier1.oldLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier1.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier1.srcStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    barrier1.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    barrier1.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
    barrier1.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
    commandBuffer->TransitionImageLayout(rendererSource->outputImageView, barrier1);

    ImageMemoryBarrier barrier3{};
    barrier3.oldLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier3.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier3.srcStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    barrier3.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    barrier3.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
    barrier3.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
    commandBuffer->TransitionImageLayout(renderpassImageViews[outImageIndex], barrier3);
    commandBuffer->BlitImage(renderpassImages[outImageIndex], rendererSource->outputImage, resolution);
    commandBuffer->TransitionImageLayout(rendererSource->outputImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    ImageMemoryBarrier barrier2{};
    barrier2.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier2.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier2.srcStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    barrier2.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    barrier2.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
    barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
    commandBuffer->TransitionImageLayout(renderpassImageViews[outImageIndex], barrier2);
    barrier2.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    commandBuffer->TransitionImageLayout(renderpassImageViews[(outImageIndex + 1) % 2], barrier2);
}
void Sandbox::PostProcess::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    for (auto& renderAttachmentContainer : renderAttachments)
    {
        for (auto& renderAttachment : renderAttachmentContainer)
        {
            renderAttachment->Cleanup();
        }
        renderAttachmentContainer.clear();
    }
    renderAttachments.clear();

    for (auto& renderTargetContainer : renderTargets)
    {
        for (auto& renderTarget : renderTargetContainer)
        {
            renderTarget->Cleanup();
        }
        renderTargetContainer.clear();
    }
    renderTargets.clear();

    sampler->Cleanup();
    for (auto& imageView : renderpassImageViews)
    {
        imageView->Cleanup();
    }
    renderpassImageViews.clear();
    for (auto& image : renderpassImages)
    {
        image->Cleanup();
    }
    renderpassImages.clear();
    // shaderLinkage->Cleanup();
    for (auto& renderpass : renderpasses)
    {
        renderpass->Cleanup();
    }
    renderpasses.clear();
}

void Sandbox::PostProcess::OnRecreateSwapchain()
{
    auto device          = m_renderer->device;
    auto resolution      = m_renderer->resolution;
    auto size            = m_renderer->swapchain->imageViews.size();
    auto postProcessSize = m_postProcessShaderSource.size();
    renderTargets.resize(postProcessSize);
    renderAttachments.resize(postProcessSize);
    for (size_t i = 0; i < postProcessSize; ++i)
    {
        renderAttachments[i].resize(size);
        renderTargets[i].resize(size);
        for (size_t j = 0; j < size; ++j)
        {
            std::vector<std::shared_ptr<ImageView>> renderAttachmentImageViews;
            renderAttachmentImageViews.push_back(renderpassImageViews[i % 2]);
            renderAttachments[i][j] = std::make_shared<RenderAttachments>(device, renderpasses[i], resolution, renderAttachmentImageViews);
            renderTargets[i][j]     = std::make_shared<RenderTarget>(device, renderpasses[i], resolution, renderAttachments[i][j]);
        }
    }
}

void Sandbox::PostProcess::AddPostProcess(const std::shared_ptr<ShaderSource>& shaderSource)
{
    if (m_renderer == nullptr)
    {
        LOGF("Engine", "Renderer is nullptr, please call Prepare first!")
    }
    // 如果不存在则添加
    if (std::find(m_postProcessShaderSource.begin(), m_postProcessShaderSource.end(), shaderSource) == m_postProcessShaderSource.end())
    {
        m_postProcessShaderSource.push_back(shaderSource);
        Cleanup();
        Prepare(m_renderer);
    }
}
void Sandbox::PostProcess::RemovePostProcess(const std::shared_ptr<ShaderSource>& shaderSource)
{
    // 在 postProcessShaderSource 中找到 shaderSource
    auto it = std::find(m_postProcessShaderSource.begin(), m_postProcessShaderSource.end(), shaderSource);
    if (it != m_postProcessShaderSource.end())
    {
        // 删除 shaderSource
        m_postProcessShaderSource.erase(it);
        Cleanup();
        // 重新准备
        Prepare(m_renderer);
    }
}
