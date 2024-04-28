#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>


namespace Sandbox
{
    class Image;
    class Device;

    class ImageView
    {
    public:
        ImageView(const std::shared_ptr<Device>& device, VkImage image, VkImageSubresource imageSubresource, VkImageViewType viewType, VkFormat inFormat);

        ImageView(const std::shared_ptr<Image>& image, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, VkFormat inFormat = VK_FORMAT_UNDEFINED,
                  uint32_t overrideMipLevels = 0);

        ~ImageView();

        void Cleanup();

        VkImageView vkImageView;

        VkFormat                format;
        VkImageSubresourceRange subresourceRange;
        VkImage                 imageReference = VK_NULL_HANDLE;

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
