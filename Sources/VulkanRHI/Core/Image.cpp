#include "Image.hpp"

#include <cassert>

#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"

VkImageType FindImageType(const VkExtent3D& extent)
{
    uint32_t dim = !!extent.width + !!extent.height + (1 < extent.depth);
    constexpr VkImageType types[] = {VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_2D, VK_IMAGE_TYPE_3D};
    assert(dim > 0 && dim < 4);
    return types[dim - 1];
}

Sandbox::Image::Image(const std::shared_ptr<Device>& device, const VkImage& image, const VkExtent3D& extent3D, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount):
    vkImage(image),
    vkDeviceMemory(nullptr),
    type(FindImageType(extent3D)),
    extent(extent3D),
    format(format),
    usage(usage),
    sampleCount(sampleCount),
    tiling(VK_IMAGE_TILING_OPTIMAL),
    arrayLayerCount(0),
    m_device(device)
{
    subresource.mipLevel = 1;
    subresource.arrayLayer = 1;

    // TODO: 可能需要 bind device memory ？？
}

Sandbox::Image::Image(const std::shared_ptr<Device>& device, const VkExtent3D& extent3D, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkSampleCountFlagBits sampleCount, VkImageTiling tiling, uint32_t mipLevels, uint32_t arrayLayers, VkImageCreateFlags flags):
    type(FindImageType(extent3D)),
    extent(extent3D),
    format(format),
    usage(usage),
    sampleCount(sampleCount),
    tiling(tiling),
    arrayLayerCount(arrayLayers),
    m_device(device)
{
    assert(0 < mipLevels && "Image should have at least one level");
    assert(0 < arrayLayers && "Image should have at least one layer");

    subresource.mipLevel = mipLevels;
    subresource.arrayLayer = arrayLayers;

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = type;
    imageCreateInfo.extent = extent;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = arrayLayers;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = sampleCount;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.flags = flags;

    m_device = device;
    auto vkDevice = m_device->vkDevice;
    if (vkCreateImage(vkDevice, &imageCreateInfo, nullptr, &vkImage) != VK_SUCCESS)
    {
        LOGF("failed to create image!")
    }
    // LOGI("{}\n{}", PtrToHexString(vkImage), GetCallStack())

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice, vkImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device->FindMemoryType(device->vkPhysicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS)
    {
        LOGF("failed to allocate image memory!")
    }

    vkBindImageMemory(vkDevice, vkImage, vkDeviceMemory, 0);
}

Sandbox::Image::~Image()
{
    Cleanup();
}

void Sandbox::Image::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkFreeMemory(m_device->vkDevice, vkDeviceMemory, nullptr);
    vkDestroyImage(m_device->vkDevice, vkImage, nullptr);
    m_cleaned = true;
}
