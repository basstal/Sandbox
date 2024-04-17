#include "pch.hpp"

#include "Renderer.hpp"

#include "Common/PipelineCaching.hpp"
#include "Common/ShaderModuleCaching.hpp"
#include "Common/SubpassComponents.hpp"
#include "Core/CommandBuffer.hpp"
#include "Core/CommandPool.hpp"
#include "Core/DescriptorPool.hpp"
#include "Core/Device.hpp"
#include "Core/Fence.hpp"
#include "Core/Instance.hpp"
#include "Core/Pipeline.hpp"
#include "Core/RenderPass.hpp"
#include "Core/Semaphore.hpp"
#include "Core/ShaderModule.hpp"
#include "Core/Surface.hpp"
#include "Core/Swapchain.hpp"
#include "Engine/EntityComponent/Components/Material.hpp"
#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "Engine/RendererSource/RendererSource.hpp"
#include "FileSystem/Directory.hpp"
#include "FileSystem/Logger.hpp"
#include "Platform/Window.hpp"
#include "Rendering/PipelineState.hpp"
#include "Rendering/RenderAttachments.hpp"
#include "Rendering/RenderTarget.hpp"

void Sandbox::Renderer::Prepare(const std::shared_ptr<Window>& window)
{
    // LOGD("Test", "测试一下中文字体")
    resolution                                = VkExtent2D{1920, 1080};
    instance                                  = std::make_shared<Instance>(VK_API_VERSION_1_0, "Sandbox");
    surface                                   = std::make_shared<Surface>(instance, window);
    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    device                                    = std::make_shared<Device>(instance, surface, deviceExtensions);
    commandPool                               = std::make_shared<CommandPool>(device);
    descriptorPool                            = std::make_shared<DescriptorPool>(device, 2048);
    swapchain                                 = std::make_shared<Swapchain>(device, surface);
    // NOTE:构造 renderpass 所需数据
    std::vector<Attachment> attachments = {
        Attachment{VK_FORMAT_R8G8B8A8_UNORM, device->GetMaxUsableSampleCount(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
        Attachment{VK_FORMAT_D32_SFLOAT_S8_UINT, device->GetMaxUsableSampleCount(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
        Attachment{VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
    };
    std::vector<LoadStoreInfo> loadStoreInfos = {
        LoadStoreInfo{VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE},
        LoadStoreInfo{VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE},
        LoadStoreInfo{VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE},
    };
    std::vector<SubpassInfo> subpassInfos = {
        SubpassInfo{{0}, {2}, false},
    };
    renderPass = std::make_shared<RenderPass>(device, attachments, loadStoreInfos, subpassInfos);
    OnAfterRecreateSwapchain();
    if (!glslang::InitializeProcess())
    {
        Logger::Fatal("Failed to initialize glslang process");
    }

    commandBuffers.resize(maxFramesFlight);
    fences.resize(maxFramesFlight);
    imageAvailableSemaphores.resize(maxFramesFlight);
    renderFinishedSemaphores.resize(maxFramesFlight);
    for (size_t i = 0; i < maxFramesFlight; ++i)
    {
        commandBuffers[i]           = std::make_shared<CommandBuffer>(device, commandPool);
        fences[i]                   = std::make_shared<Fence>(device);
        imageAvailableSemaphores[i] = std::make_shared<Semaphore>(device);
        renderFinishedSemaphores[i] = std::make_shared<Semaphore>(device);
    }

    swapchain->onAfterRecreateSwapchain.BindMember<Sandbox::Renderer, &Renderer::OnAfterRecreateSwapchain>(this);
    onViewModeChanged.BindMember<Renderer, &Renderer::OnViewModeChanged>(this);
    pipelineCaching     = std::make_shared<PipelineCaching>();
    shaderModuleCaching = std::make_shared<ShaderModuleCaching>(device);
}

void Sandbox::Renderer::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    shaderModuleCaching->Cleanup();
    pipelineCaching->Cleanup();

    for (size_t i = 0; i < maxFramesFlight; ++i)
    {
        renderFinishedSemaphores[i]->Cleanup();
        imageAvailableSemaphores[i]->Cleanup();
        fences[i]->Cleanup();
        commandBuffers[i]->Cleanup();
    }

    for (auto& mappingPair : rendererSourceMapping)
    {
        mappingPair.second->Cleanup();
    }

    glslang::FinalizeProcess();
    for (auto& renderTarget : renderTargets)
    {
        renderTarget->Cleanup();
    }
    for (auto& renderAttachment : renderAttachments)
    {
        renderAttachment->Cleanup();
    }
    renderPass->Cleanup();
    swapchain->Cleanup();
    descriptorPool->Cleanup();
    commandPool->Cleanup();
    device->Cleanup();
    surface->Cleanup();
    instance->Cleanup();
    m_cleaned = true;
}

Sandbox::ESwapchainStatus Sandbox::Renderer::AcquireNextImage()
{
    // fences[frameFlightIndex]->WaitForFence();
    return swapchain->AcquireNextImageIndex(imageAvailableSemaphores[frameFlightIndex]);
}

void Sandbox::Renderer::Draw()
{
    std::shared_ptr<CommandBuffer>& commandBuffer = commandBuffers[frameFlightIndex];
    commandBuffer->Reset();
    RecordCommandBuffer(commandBuffer);
    commandBuffer->Submit(fences[frameFlightIndex], {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {}, {});
    // 等待这个栅栏是为了让其他 commandBuffer 能够用场景输出的数据，例如 color attachment
    fences[frameFlightIndex]->WaitForFence();
}

void Sandbox::Renderer::Preset()
{
    // onOtherCommandBuffer.Trigger(frameFlightIndex);

    swapchain->Preset({renderFinishedSemaphores[frameFlightIndex]});
    FrameFlightIndexIncrease();
}

void Sandbox::Renderer::FrameFlightIndexIncrease() { frameFlightIndex = (frameFlightIndex + 1) % maxFramesFlight; }

void Sandbox::Renderer::RecordCommandBuffer(std::shared_ptr<CommandBuffer>& commandBuffer)
{
    commandBuffer->Begin();
    auto                            imageIndex        = swapchain->acquiredNextImageIndex;
    std::shared_ptr<Framebuffer>&   framebuffer       = renderTargets[imageIndex]->framebuffer;
    VkClearColorValue               clearColor        = {{0.8f, 0.8f, 0.8f, 1.0f}};
    VkClearDepthStencilValue        clearDepthStencil = {1.0f, 0};
    std::shared_ptr<RendererSource> rendererSource;
    if (!TryGetRendererSource(viewMode, rendererSource))
    {
        LOGF_OLD("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(viewMode)])
    }
    // 绘制场景
    {
        commandBuffer->BeginRenderPass(renderPass, framebuffer, resolution, clearColor, clearDepthStencil);

        VkViewport viewport;
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(resolution.width);
        viewport.height   = static_cast<float>(resolution.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        commandBuffer->SetViewport(0, std::vector<VkViewport>{viewport});

        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = resolution;
        commandBuffer->SetScissor(0, std::vector<VkRect2D>{scissor});

        onBeforeRendererDraw.Trigger(commandBuffer, frameFlightIndex);

        commandBuffer->BindPipeline(pipeline);

        uint32_t dynamicAlignment = GetUniformDynamicAlignment(sizeof(glm::mat4));
        uint32_t offset           = 0;
        for (auto& material : queuedMaterials)
        {
            if (material != nullptr)
            {
                // onBeforeDrawMesh.Trigger(commandBuffer, frameFlightIndex, nextMesh);
                material->DrawMesh(pipeline->pipelineLayout, rendererSource, frameFlightIndex, commandBuffer, offset++ * dynamicAlignment);
                // onAfterDrawMesh.Trigger(commandBuffer, frameFlightIndex, nextMesh);
            }
        }
        offset = 0;
        for (auto& material : queuedMaterials)
        {
            if (material != nullptr)
            {
                // onBeforeDrawMesh.Trigger(commandBuffer, frameFlightIndex, nextMesh);
                material->DrawOverlay(pipeline->pipelineLayout, rendererSource, frameFlightIndex, commandBuffer, offset++ * dynamicAlignment);
                // onAfterDrawMesh.Trigger(commandBuffer, frameFlightIndex, nextMesh);
            }
        }
        queuedMaterials.clear();
        onAfterRendererDraw.Trigger(commandBuffer, frameFlightIndex);

        commandBuffer->EndRenderPass();
    }

    onOtherDrawCommands.Trigger(commandBuffer, frameFlightIndex);
    // 在这里要保证游戏场景的 renderpass 输出完毕并且 image layout 也转换为纹理采样所需的 image，因为这里会 submit 并等待其他的 viewport 的渲染
    commandBuffer->TransitionImageLayout(renderAttachments[imageIndex]->resolveImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    commandBuffer->End();
}

void Sandbox::Renderer::UpdateUniforms(std::shared_ptr<CommandBuffer>& commandBuffer, std::shared_ptr<RendererSource>& rendererSource) {}

void Sandbox::Renderer::OnAfterRecreateSwapchain()
{
    // TODO:是否要创建多个？
    renderTargets.resize(swapchain->vkImages.size());
    renderAttachments.resize(renderTargets.size());
    for (size_t i = 0; i < renderTargets.size(); ++i)
    {
        renderAttachments[i] != nullptr ? renderAttachments[i]->Cleanup() : void();
        renderTargets[i] != nullptr ? renderTargets[i]->Cleanup() : void();
        renderAttachments[i] = std::make_shared<RenderAttachments>(device, renderPass, resolution, nullptr);
        renderTargets[i]     = std::make_shared<RenderTarget>(device, renderPass, resolution, renderAttachments[i]);
    }
}
uint32_t Sandbox::Renderer::GetUniformDynamicAlignment(VkDeviceSize dynamicAlignment) const
{
    device->GetMinUniformBufferOffsetAlignment(dynamicAlignment);
    return static_cast<uint32_t>(dynamicAlignment);
}
bool Sandbox::Renderer::TryGetRendererSource(EViewMode inViewMode, std::shared_ptr<RendererSource>& outRendererSource)
{
    if (!rendererSourceMapping.contains(inViewMode))
    {
        return false;
    }
    outRendererSource = rendererSourceMapping[inViewMode];
    return true;
}


void Sandbox::Renderer::OnViewModeChanged(EViewMode inViewMode)
{
    viewMode = inViewMode;
    std::shared_ptr<RendererSource> rendererSource;
    if (!TryGetRendererSource(viewMode, rendererSource))
    {
        LOGF_OLD("Renderer source for view mode '{}' not found", VIEW_MODE_NAMES[static_cast<uint32_t>(viewMode)])
	}
	auto shaderModules = rendererSource->shaderModules;
	pipeline = rendererSource->pipeline;
}

	