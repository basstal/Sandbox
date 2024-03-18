#pragma once
#include <memory>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>


namespace Sandbox
{
    namespace Resource
    {
        class Image;
    }

    class Image;
    class Device;
    class Pipeline;
    class CommandPool;
    class RenderPass;
    class Framebuffer;
    class DescriptorSet;
    class Buffer;
    class Fence;
    class Semaphore;

    class CommandBuffer
    {
    public:
        CommandBuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandPool>& commandPool);

        ~CommandBuffer();

        void Cleanup();

        void Reset();

        void Begin(VkCommandBufferUsageFlags flags = 0);

        void End();

        void EndAndSubmit();

        void BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<Framebuffer>& framebuffer, VkExtent2D vkExtent2D, VkClearColorValue clearColor,
                             VkClearDepthStencilValue clearDepthStencil);

        void EndRenderPass();

        void SetViewport(uint32_t firstViewport, const std::vector<VkViewport>& viewports);

        void SetScissor(uint32_t firstScissor, const std::vector<VkRect2D>& scissors);

        void BindPipeline(const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<DescriptorSet>& descriptorSet, const std::vector<uint32_t>& dynamicOffsets);

        void BindVertexBuffers(const std::shared_ptr<Buffer>& buffer);

        void BindIndexBuffer(const std::shared_ptr<Buffer>& buffer);

        void DrawIndexed(uint32_t indexCount);

        void Draw(uint32_t count);

        void CopyDataToBuffer(const void* inData, VkDeviceSize size, const std::shared_ptr<Buffer>& dstBuffer);

        void CopyDataToImage(const std::shared_ptr<Resource::Image>& imageResource, const std::shared_ptr<Image>& vkImage, VkFormat format);

        void GenerateMipmaps(VkImage vkImage, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

        void CopyBufferToImage(Buffer& buffer, VkImage vkImage, uint32_t width, uint32_t height);

        void TransitionImageLayout(const std::shared_ptr<Image>& vkImage, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1);

        void Submit(const std::shared_ptr<Fence>& fence, const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<std::shared_ptr<Semaphore>>& waitSemaphores,
                    const std::vector<std::shared_ptr<
                        Semaphore>>& signalSemaphores);

        void BufferMemoryBarrier(const std::shared_ptr<Buffer>& buffer);

        void ImageMemoryBarrier(const std::shared_ptr<Image>& image);

        void BlitImage(const std::shared_ptr<Image>& srcImage, const std::shared_ptr<Image>& dstImage, VkExtent2D size);

        VkCommandBuffer vkCommandBuffer;

    private:
        std::shared_ptr<CommandPool> m_commandPool;
        std::shared_ptr<Device> m_device;
        bool m_cleaned = false;
    };
}
