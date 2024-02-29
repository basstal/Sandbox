#include "UniformCombinedImage.hpp"

#include "Image.hpp"
#include "Rendering/Objects/Sampler.hpp"
#include "Rendering/Components/Swapchain.hpp"

UniformCombinedImage::UniformCombinedImage(const std::shared_ptr<Device>& device, VkSamplerAddressMode samplerAddressMode)
{
    m_device = device;
    sampler = std::make_shared<Sampler>(device, samplerAddressMode);
}

UniformCombinedImage::~UniformCombinedImage()
{
    Cleanup();
}

void UniformCombinedImage::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
                                       VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool isCubeMap, VkImageAspectFlags aspectFlags)
{
    images.push_back(std::make_shared<Image>(m_device, width, height, mipLevels, numSamples, format, tiling, usage, properties, isCubeMap, aspectFlags));
}

void UniformCombinedImage::AddImage(const std::shared_ptr<Image>& image)
{
    images.push_back(image);
}

std::vector<VkDescriptorImageInfo> UniformCombinedImage::CreateDescriptorImageInfos()
{
    std::vector<VkDescriptorImageInfo> result;
    for (auto& image : images)
    {
        VkDescriptorImageInfo descriptorImageInfo{};
        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = image->vkImageView;
        descriptorImageInfo.sampler = sampler->vkSampler;
        result.push_back(descriptorImageInfo);
    }
    return result;
}

VkWriteDescriptorSet UniformCombinedImage::CreateWriteDescriptorSet(uint32_t binding, VkDescriptorSet descriptorSet, const std::vector<VkDescriptorImageInfo>& descriptorImageInfos)
{
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorImageInfos.size());
    writeDescriptorSet.pImageInfo = descriptorImageInfos.data();

    return writeDescriptorSet;
}

void UniformCombinedImage::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    for (auto& image : images)
    {
        image->Cleanup();
    }
    sampler->Cleanup();
    m_cleaned = true;
}
