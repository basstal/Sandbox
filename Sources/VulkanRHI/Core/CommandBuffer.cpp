#include "pch.hpp"

#include "CommandBuffer.hpp"

#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "DescriptorSet.hpp"
#include "Device.hpp"
#include "Engine/Image.hpp"
#include "Fence.hpp"
#include "FileSystem/Logger.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "Semaphore.hpp"
#include "VulkanRHI/Common/Debug.hpp"
#include "VulkanRHI/Common/SubpassComponents.hpp"
#include "VulkanRHI/Rendering/PipelineState.hpp"

Sandbox::CommandBuffer::CommandBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandPool>& commandPool)
{
    m_device      = device;
    m_commandPool = commandPool;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool->vkCommandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(m_device->vkDevice, &allocInfo, &vkCommandBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate command buffers!");
    }
    LOGI_OLD("{}\n{}", PtrToHexString(vkCommandBuffer), GetCallStack())
}

Sandbox::CommandBuffer::~CommandBuffer() { Cleanup(); }

void Sandbox::CommandBuffer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkFreeCommandBuffers(m_device->vkDevice, m_commandPool->vkCommandPool, 1, &vkCommandBuffer);
    m_cleaned = true;
}

void Sandbox::CommandBuffer::Reset() { vkResetCommandBuffer(vkCommandBuffer, 0); }

void Sandbox::CommandBuffer::Begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = flags;
    beginInfo.pInheritanceInfo = nullptr;  // Optional

    if (vkBeginCommandBuffer(vkCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        Logger::Fatal("failed to begin recording command buffer!");
    }
}

void Sandbox::CommandBuffer::End()
{
    if (vkEndCommandBuffer(vkCommandBuffer) != VK_SUCCESS)
    {
        Logger::Fatal("failed to record command buffer!");
    }
}

void Sandbox::CommandBuffer::EndAndSubmit()
{
    End();
    Submit(nullptr, {}, {}, {});
    if (vkQueueWaitIdle(m_device->graphicsQueue) != VK_SUCCESS)
    {
        LOGF_OLD("failed to wait for queue idle!")
    }
}

void Sandbox::CommandBuffer::BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<Framebuffer>& framebuffer, VkExtent2D vkExtent2D,
                                             VkClearColorValue clearColor, VkClearDepthStencilValue clearDepthStencil)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPass->vkRenderPass;
    renderPassInfo.framebuffer       = framebuffer->vkFramebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkExtent2D;
    std::vector<VkClearValue> clearValues(renderPass->loadStoreInfo.size());
    for (size_t i = 0; i < clearValues.size(); ++i)
    {
        if (renderPass->loadStoreInfo[i].loadOp != VK_ATTACHMENT_LOAD_OP_CLEAR)
        {
            continue;
        }
        if (renderPass->attachments[i].usage == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            clearValues[i].depthStencil = clearDepthStencil;
        }
        else
        {
            clearValues[i].color = clearColor;
        }
    }
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues    = clearValues.data();
    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Sandbox::CommandBuffer::EndRenderPass() { vkCmdEndRenderPass(vkCommandBuffer); }

void Sandbox::CommandBuffer::SetViewport(uint32_t firstViewport, const std::vector<VkViewport>& viewports)
{
    vkCmdSetViewport(vkCommandBuffer, firstViewport, static_cast<uint32_t>(viewports.size()), viewports.data());
}

void Sandbox::CommandBuffer::SetScissor(uint32_t firstScissor, const std::vector<VkRect2D>& scissors)
{
    vkCmdSetScissor(vkCommandBuffer, firstScissor, static_cast<uint32_t>(scissors.size()), scissors.data());
}


void Sandbox::CommandBuffer::BindDescriptorSet(const std::shared_ptr<PipelineLayout>& pipelineLayout, const std::shared_ptr<DescriptorSet>& descriptorSet,
                                               const std::vector<uint32_t>& dynamicOffsets)
{
    descriptorSet->Update();
    vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->vkPipelineLayout, 0, 1, &descriptorSet->vkDescriptorSet,
                            static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
}

void Sandbox::CommandBuffer::BindPipeline(const std::shared_ptr<Pipeline>& pipeline)
{
    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
    // PushConstants(pipeline->pipelineLayout->vkPipelineLayout, pushConstantsInfo);
    m_boundPipeline = pipeline;
}

void Sandbox::CommandBuffer::PushConstants(const PushConstantsInfo& pushConstantsInfo)
{
    // auto& pipelineState = m_boundPipeline->pipelineState;
    // auto& pushConstantsInfo = pipelineState->pushConstantsInfo;
    // auto& pipelineLayout = pipelineState->pipelineLayout;
    // TODO:与 m_boundPipeline->pipelineLayout->pushConstantRanges 对齐
    if (pushConstantsInfo.size > 0 && pushConstantsInfo.data != nullptr)
    {
        vkCmdPushConstants(vkCommandBuffer, m_boundPipeline->pipelineLayout->vkPipelineLayout, pushConstantsInfo.stage, 0, pushConstantsInfo.size, pushConstantsInfo.data);
    }
}

void Sandbox::CommandBuffer::BindVertexBuffers(const std::shared_ptr<Buffer>& buffer)
{
    VkBuffer     vertexBuffers[] = {buffer->vkBuffer};
    VkDeviceSize offsets[]       = {0};
    vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
}

void Sandbox::CommandBuffer::BindIndexBuffer(const std::shared_ptr<Buffer>& buffer) { vkCmdBindIndexBuffer(vkCommandBuffer, buffer->vkBuffer, 0, VK_INDEX_TYPE_UINT32); }

void Sandbox::CommandBuffer::DrawIndexed(uint32_t indexCount) { vkCmdDrawIndexed(vkCommandBuffer, indexCount, 1, 0, 0, 0); }

void Sandbox::CommandBuffer::Draw(uint32_t count) { vkCmdDraw(vkCommandBuffer, count, 1, 0, 0); }


void Sandbox::CommandBuffer::CopyDataToBuffer(const void* inData, VkDeviceSize size, const std::shared_ptr<Buffer>& dstBuffer)
{
    Buffer stagingBuffer(m_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.Update(inData);
    Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size      = size;
    vkCmdCopyBuffer(vkCommandBuffer, stagingBuffer.vkBuffer, dstBuffer->vkBuffer, 1, &copyRegion);
    EndAndSubmit();
}

void Sandbox::CommandBuffer::CopyDataToImage(const std::shared_ptr<Resource::Image>& imageResource, const std::shared_ptr<Image>& image, VkFormat format)
{
    VkDeviceSize imageSize = (VkDeviceSize)imageResource->width * imageResource->height * sizeof(float);
    Buffer       stagingBuffer(m_device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.Update(imageResource->pixels);

    Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageResource->mipLevels);
    auto vkImage = image->vkImage;
    CopyBufferToImage(stagingBuffer, vkImage, static_cast<uint32_t>(imageResource->width), static_cast<uint32_t>(imageResource->height));
    GenerateMipmaps(vkImage, format, imageResource->width, imageResource->height, imageResource->mipLevels);
    EndAndSubmit();
}

void Sandbox::CommandBuffer::GenerateMipmaps(VkImage vkImage, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_device->vkPhysicalDevice, imageFormat, &formatProperties);
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        Logger::Fatal("texture image format does not support linear blitting!");
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image                           = vkImage;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    int32_t mipWidth  = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0]                 = {0, 0, 0};
        blit.srcOffsets[1]                 = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel       = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount     = 1;
        blit.dstOffsets[0]                 = {0, 0, 0};
        blit.dstOffsets[1]                 = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel       = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount     = 1;

        vkCmdBlitImage(vkCommandBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        if (mipWidth > 1)
        {
            mipWidth /= 2;
        }
        if (mipHeight > 1)
        {
            mipHeight /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Sandbox::CommandBuffer::CopyBufferToImage(Buffer& buffer, VkImage vkImage, uint32_t width, uint32_t height)
{
    VkBufferImageCopy region;
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};
    vkCmdCopyBufferToImage(vkCommandBuffer, buffer.vkBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void Sandbox::CommandBuffer::TransitionImageLayout(const std::shared_ptr<Image>& vkImage, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkImageMemoryBarrier barrier{};
    VkPipelineStageFlags sourceStage{}, destinationStage{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = vkImage->vkImage;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    // TODO:重构成可查询的数据结构
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
    {
        if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        }
        else
        {
            Logger::Fatal("unsupported new layout {} from VK_IMAGE_LAYOUT_UNDEFINED!", static_cast<uint32_t>(newLayout));
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            Logger::Fatal("unsupported new layout {} from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL!", static_cast<uint32_t>(newLayout));
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            Logger::Fatal("unsupported new layout {} from VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL!", static_cast<uint32_t>(newLayout));
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  // 第一个渲染通道结束时的访问类型
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;  // 第二个渲染通道开始前的访问类型

            sourceStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  // 第一个渲染通道的最后一个阶段
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // 第二个渲染通道的第一个阶段
        }
        else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            // TODO:
            // barrier.srcAccessMask = 0;
            // barrier.dstAccessMask = 0;
            //
            // sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // 第一个渲染通道的最后一个阶段
            // destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // 第二个渲染通道的第一个阶段
        }
        else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  // 第一个渲染通道的最后一个阶段
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;  // 第二个渲染通道的第一个阶段
        }
        else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  // 第一个渲染通道的最后一个阶段
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;  // 第二个渲染通道的第一个阶段
        }
        else
        {
            Logger::Fatal("unsupported new layout {} from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL!", static_cast<uint32_t>(newLayout));
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // 在片段着色器读取完成后
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  // 在颜色附件输出之前
    }
    else
    {
        Logger::Fatal("unsupported layout transition!");
    }
    vkCmdPipelineBarrier(vkCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Sandbox::CommandBuffer::Submit(const std::shared_ptr<Fence>& fence, const std::vector<VkPipelineStageFlags>& waitStages,
                                    const std::vector<std::shared_ptr<Semaphore>>& waitSemaphores, const std::vector<std::shared_ptr<Semaphore>>& signalSemaphores)
{
    fence != nullptr ? fence->Reset() : void();
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    std::vector<VkSemaphore> vkWaitSemaphores;
    for (auto it = waitSemaphores.cbegin(); it != waitSemaphores.cend(); ++it)
    {
        vkWaitSemaphores.push_back((*it)->vkSemaphore);
    }
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vkWaitSemaphores.size());
    submitInfo.pWaitSemaphores    = vkWaitSemaphores.data();
    submitInfo.pWaitDstStageMask  = waitStages.empty() ? nullptr : waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &vkCommandBuffer;
    std::vector<VkSemaphore> vkSignalSemaphores;
    for (auto it = signalSemaphores.cbegin(); it != signalSemaphores.cend(); ++it)
    {
        vkSignalSemaphores.push_back((*it)->vkSemaphore);
    }
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vkSignalSemaphores.size());
    submitInfo.pSignalSemaphores    = vkSignalSemaphores.data();
    ValidateVkResult(vkQueueSubmit(m_device->graphicsQueue, 1, &submitInfo, fence == nullptr ? nullptr : fence->vkFence));
}

void Sandbox::CommandBuffer::BufferMemoryBarrier(const std::shared_ptr<Buffer>& buffer)
{
    VkBufferMemoryBarrier bufferMemoryBarrier{};
    bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    // 设置此屏障是为了等待主机写入
    bufferMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    // 当我们在顶点着色器中读取数据时，需要这个访问类型
    bufferMemoryBarrier.dstAccessMask       = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;  // 不跨队列家族转换
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;  // 不跨队列家族转换
    bufferMemoryBarrier.buffer              = buffer->vkBuffer;  // 你要更新的uniform buffer
    bufferMemoryBarrier.offset              = 0;  // 从哪里开始
    bufferMemoryBarrier.size                = VK_WHOLE_SIZE;  // 整个buffer大小
    VkPipelineStageFlags sourceStage        = VK_PIPELINE_STAGE_HOST_BIT;
    VkPipelineStageFlags destinationStage   = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    vkCmdPipelineBarrier(vkCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
}

void Sandbox::CommandBuffer::ImageMemoryBarrier(const std::shared_ptr<Image>& image)
{
    // TODO:这应该是一个 image layout transition
    // VkImageMemoryBarrier imageMemoryBarrier{};
    // imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    // imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    // imageMemoryBarrier.image = image->vkImage;
    // imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    // imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    // imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    // imageMemoryBarrier.subresourceRange.layerCount = 1;
    // imageMemoryBarrier.subresourceRange.levelCount = 1;
    // imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    // VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    // vkCmdPipelineBarrier(vkCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void Sandbox::CommandBuffer::BlitImage(const std::shared_ptr<Image>& srcImage, const std::shared_ptr<Image>& dstImage, VkExtent2D size)
{
    VkImageBlit imageBlit{};

    // 源图像的指定
    imageBlit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;  // 指定操作的是颜色数据
    imageBlit.srcSubresource.mipLevel       = 0;  // 源图像的 mip 级别
    imageBlit.srcSubresource.baseArrayLayer = 0;  // 起始的数组层
    imageBlit.srcSubresource.layerCount     = 1;  // 操作的图像层数

    // 源图像的拷贝区域
    imageBlit.srcOffsets[0] = {0, 0, 0};  // 起始点
    imageBlit.srcOffsets[1] = VkOffset3D{static_cast<int32_t>(size.width), static_cast<int32_t>(size.height), 1};  // 终点，假设是2D图像，Z维度设置为1

    // 目标图像的指定
    imageBlit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBlit.dstSubresource.mipLevel       = 0;
    imageBlit.dstSubresource.baseArrayLayer = 0;
    imageBlit.dstSubresource.layerCount = 1;

    // 目标图像的拷贝区域
    imageBlit.dstOffsets[0] = {0, 0, 0}; // 目标起始点
    imageBlit.dstOffsets[1] = VkOffset3D{static_cast<int32_t>(size.width), static_cast<int32_t>(size.height), 1}; // 目标终点，同样假设是2D图像

    vkCmdBlitImage(
        vkCommandBuffer,
        srcImage->vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, // 源图像及其布局
        dstImage->vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // 目标图像及其布局
        1, &imageBlit, // Blit操作的数量和具体操作
        VK_FILTER_LINEAR // 使用线性滤波
    );
}

std::shared_ptr<Sandbox::Pipeline> Sandbox::CommandBuffer::GetBoundPipeline(){
    return m_boundPipeline;
}
