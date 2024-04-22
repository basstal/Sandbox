#include "pch.hpp"

#include "BufferRendererSource.hpp"

#include "FileSystem/Directory.hpp"
#include "Misc/TypeCasting.hpp"
#include "VulkanRHI/Common/Caching/DescriptorSetCaching.hpp"
#include "VulkanRHI/Common/Caching/PipelineCaching.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Core/Buffer.hpp"
#include "VulkanRHI/Core/CommandBuffer.hpp"
#include "VulkanRHI/Core/DescriptorSet.hpp"
#include "VulkanRHI/Core/Device.hpp"
#include "VulkanRHI/Core/Framebuffer.hpp"
#include "VulkanRHI/Core/Image.hpp"
#include "VulkanRHI/Core/ImageView.hpp"
#include "VulkanRHI/Core/Pipeline.hpp"
#include "VulkanRHI/Core/PipelineLayout.hpp"
#include "VulkanRHI/Renderer.hpp"
#include "VulkanRHI/Rendering/RenderTarget.hpp"
#include "VulkanRHI/Rendering/ShaderLinkage.hpp"


std::vector<Sandbox::SimpleVertexFlat> GetQuadProperties()
{
    // 构造 vulkan 全屏四边形
    std::vector<Sandbox::SimpleVertexFlat> quadBuffer = {
        {{-1.0f, 1.0f}, {0.0f, 0.0f}}, {{1.0f, -1.0f}, {1.0f, 1.0f}}, {{-1.0f, -1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f}, {0.0f, 0.0f}}, {{1.0f, 1.0f}, {1.0f, 0.0f}},  {{1.0f, -1.0f}, {1.0f, 1.0f}},
    };
    return quadBuffer;
}

void Sandbox::BufferRendererSource::Prepare(std::shared_ptr<Renderer>& renderer)
{
    PbrRendererSource::Prepare(renderer);
    m_renderer      = renderer;
    auto device     = renderer->device;
    auto resolution = renderer->resolution;

    auto resolution3D = VkExtent3D{resolution.width, resolution.height, 1};
    if (bufferType == BufferType::Depth)
    {
        std::vector<Attachment> attachments = {
            Attachment{VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
            Attachment{VK_FORMAT_D32_SFLOAT_S8_UINT, device->GetMaxUsableSampleCount(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
        };
        std::vector<LoadStoreInfo> loadStoreInfos = {
            LoadStoreInfo{VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE},
            LoadStoreInfo{VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE},
        };
        std::vector<SubpassInfo> subpassInfos;
        SubpassInfo              subpass;
        subpass.colorAttachments.push_back(0);
        subpass.inputAttachments.push_back(1);
        subpassInfos.emplace_back(subpass);
        blitDepthRenderPass = std::make_shared<RenderPass>(renderer->device, attachments, loadStoreInfos, subpassInfos);

        // 使用图形管线：设置一个渲染管线，其中源图像作为深度附件，目标图像作为颜色附件。在片段着色器中，你可以根据深度值计算颜色，并输出到颜色附件。
        blitDepthShaderLinkage = std::make_shared<ShaderLinkage>();
        auto assetDirectory    = Directory::GetAssetsDirectory();
        auto vertexSource      = std::make_shared<ShaderSource>(assetDirectory.GetFile("Shaders/BlitDepth.vert"), "");
        auto fragmentSource    = std::make_shared<ShaderSource>(assetDirectory.GetFile("Shaders/BlitDepth.frag"), "");
        blitDepthShaderLinkage->CreateShaderModule(renderer, VK_SHADER_STAGE_VERTEX_BIT, vertexSource);
        blitDepthShaderLinkage->CreateShaderModule(renderer, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentSource);
        blitDepthPipelineState = std::make_shared<PipelineState>(blitDepthShaderLinkage, blitDepthRenderPass);
        blitDepthPipeline      = renderer->pipelineCaching->GetOrCreatePipeline(blitDepthPipelineState);
        // outputImage = std::make_shared<Image>(renderer->device, resolution3D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        //                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        // blitDepthDestinationImage = std::make_shared<Image>(device, resolution3D, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        //                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        blitDepthDescriptorSets.resize(renderer->maxFramesFlight);
        for (uint32_t i = 0; i < renderer->maxFramesFlight; ++i)
        {
            blitDepthDescriptorSets[i] = renderer->descriptorSetCaching->GetOrCreateDescriptorSet(blitDepthPipeline->pipelineLayout->descriptorSetLayout, i);
        }
        // uint32_t frameFlightSize = renderer->maxFramesFlight;
        // // uint32_t dynamicAlignment = renderer->GetUniformDynamicAlignment(sizeof(glm::mat4));
        //
        // for (size_t i = 0; i < frameFlightSize; ++i)
        // {
        //
        // }
        quadProperties  = GetQuadProperties();
        auto bufferSize = sizeof(SimpleVertexFlat) * quadProperties.size();
        quadBuffer = std::make_shared<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        renderer->commandBuffers[0]->CopyDataToBuffer(quadProperties.data(), bufferSize, quadBuffer);
        OnRecreateSwapchain();
    }
}


void Sandbox::BufferRendererSource::Cleanup()
{
    PbrRendererSource::Cleanup();
    for (auto& blitDepthAttachment : blitDepthAttachments)
    {
        blitDepthAttachment->Cleanup();
    }
    for (auto& blitDepthRenderTarget : blitDepthRenderTargets)
    {
        blitDepthRenderTarget->Cleanup();
    }
    for (auto& blitDepthDescriptorSet : blitDepthDescriptorSets)
    {
        blitDepthDescriptorSet->Cleanup();
    }

    // blitDepthDestinationImage->Cleanup();
    // outputImageView->Cleanup();
    // outputImage->Cleanup();
    blitDepthShaderLinkage->Cleanup();
    blitDepthRenderPass->Cleanup();
}
void Sandbox::BufferRendererSource::BlitImage(const std::shared_ptr<CommandBuffer>& commandBuffer, const std::shared_ptr<RenderAttachments>& renderAttachments,
                                              VkExtent2D resolution)
{
    // PbrRendererSource::BlitImage(commandBuffer, renderAttachments, resolution);
    if (bufferType == BufferType::Depth)
    {
        // 转换多采样深度图的布局
        commandBuffer->TransitionImageLayout(renderAttachments->images[1], VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        VkClearColorValue        clearColor        = {{0.0f, 0.0f, 0.0f, 1.0f}};
        VkClearDepthStencilValue clearDepthStencil = {1.0f, 0};
        commandBuffer->BeginRenderPass(blitDepthRenderPass, blitDepthRenderTargets[m_renderer->swapchain->acquiredNextImageIndex]->framebuffer, resolution, clearColor,
                                       clearDepthStencil);
        commandBuffer->BindPipeline(blitDepthPipeline);

        // 动态修改资源绑定
        {
            VkDescriptorImageInfo imageInfo;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = renderAttachments->imageViews[1]->vkImageView;
            imageInfo.sampler     = VK_NULL_HANDLE;

            std::map<uint32_t, std::vector<VkDescriptorImageInfo>> imageInfoMapping{{0, {imageInfo}}};

            blitDepthDescriptorSets[m_renderer->frameFlightIndex]->BindImageInfoMapping(imageInfoMapping, blitDepthPipeline->pipelineLayout->descriptorSetLayout);
        }
        commandBuffer->BindDescriptorSet(blitDepthPipeline->pipelineLayout, blitDepthDescriptorSets[m_renderer->frameFlightIndex]);

        commandBuffer->BindVertexBuffers(quadBuffer);
        commandBuffer->Draw(ToUInt32(quadProperties.size()));
        commandBuffer->EndRenderPass();
        // commandBuffer->TransitionImageLayout(renderAttachments->images[1], VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }
}
void Sandbox::BufferRendererSource::OnRecreateSwapchain()
{
    PbrRendererSource::OnRecreateSwapchain();
    auto device     = m_renderer->device;
    auto resolution = m_renderer->resolution;
    if (bufferType == BufferType::Depth)
    {
        // outputImageView = std::make_shared<ImageView>(outputImage, VK_IMAGE_VIEW_TYPE_2D);
        auto size = m_renderer->swapchain->imageViews.size();
        blitDepthRenderTargets.resize(size);
        blitDepthAttachments.resize(size);
        for (size_t i = 0; i < size; ++i)
        {
            std::vector<std::shared_ptr<ImageView>> renderAttachmentImageViews;
            renderAttachmentImageViews.resize(blitDepthRenderPass->attachments.size());
            renderAttachmentImageViews[0] = outputImageView;
            renderAttachmentImageViews[1] = m_renderer->renderAttachments[i]->imageViews[1];
            blitDepthAttachments[i]       = std::make_shared<RenderAttachments>(device, blitDepthRenderPass, resolution, renderAttachmentImageViews);
            blitDepthRenderTargets[i]     = std::make_shared<RenderTarget>(device, blitDepthRenderPass, resolution, blitDepthAttachments[i]);
        }
    }
}

