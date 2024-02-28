#pragma once
#include <memory>
#include <vulkan/vulkan_core.h>

#include "Rendering/Base/Device.hpp"
#include "Rendering/Components/CommandResource.hpp"

namespace GameCore
{
    class Image;
}

class Image;
class RenderPass;

class Framebuffer
{
private:
    struct SupportImageProperty
    {
        uint32_t width;
        uint32_t height;
        bool isHdrImage;
        uint32_t mipLevels;
    };

    void CopyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height);

    void GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void CreateTextureSampler(VkSamplerAddressMode samplerAddressMode);

    std::shared_ptr<CommandResource> m_commandPool;
    std::shared_ptr<Device> m_device;

    bool m_cleaned = false;
    SupportImageProperty m_supportImageProperty;

    bool IsSupportImageProperty(uint32_t width, uint32_t height, bool isHdrImage);

public:
    // VkImage vkImage;
    // VkDeviceMemory vkDeviceMemory;
    VkImageView vkImageView;
    VkSampler vkSampler;
    VkFormat vkFormat;
    VkFramebuffer vkFramebuffer;
    std::shared_ptr<Image> image;

    Framebuffer(const std::shared_ptr<Device>& device, const std::shared_ptr<CommandResource>& commandResource, bool isCubeMap, bool isHdrImage, uint32_t width, uint32_t height,
                uint32_t mipLevels);

    Framebuffer(const std::shared_ptr<Device>& device, VkRenderPass vkRenderPass, const std::vector<VkImageView>& attachments, VkExtent2D vkExtent2D);


    ~Framebuffer();

    void AssignImageData(const std::shared_ptr<GameCore::Image>& image);

    void Cleanup();

    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

    void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    // void CreateFramebuffers(const std::shared_ptr<RenderPass>& renderPass);

    // VkFramebuffer CreateFramebuffer(VkRenderPass vkRenderPass, const std::vector<VkImageView>& attachments, VkExtent2D vkExtent2D);
};
