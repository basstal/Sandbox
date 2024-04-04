#pragma once
#include <memory>

#include "vulkan/vulkan_core.h"

namespace Sandbox
{
    class Device;
    class Sampler;
    class Image;
    class ImageView;

    class Texture
    {
    public:
        std::shared_ptr<Sampler>   sampler;
        std::shared_ptr<Image>     image;
        std::shared_ptr<ImageView> imageView;

        Texture(std::shared_ptr<Image>&& inImage, VkSamplerAddressMode samplerAddressMode);

        Texture(const std::shared_ptr<Device> device, VkSamplerAddressMode samplerAddressMode);

        ~Texture();

        VkDescriptorImageInfo GetDescriptorImageInfo();

        void Cleanup();

    private:
        std::shared_ptr<Device> m_device;
        bool                    m_cleaned = false;
    };
}  // namespace Sandbox
