#include "DescriptorResource.hpp"

#include <array>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Swapchain.hpp"
#include "Infrastructures/FileSystem/Logger.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Base/Device.hpp"

VkDescriptorPool DescriptorResource::vkDescriptorPool = nullptr;

DescriptorResource::DescriptorResource(const std::shared_ptr<Device>& device):
    m_device(device), vkDescriptorSetLayout(VK_NULL_HANDLE)
{
}

DescriptorResource::~DescriptorResource()
{
    Cleanup();
}

void DescriptorResource::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(m_device->vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create descriptor set layout!");
    }
}

void DescriptorResource::CreateDescriptorPool(const std::shared_ptr<Device>& device)
{
    if (vkDescriptorPool != nullptr)
    {
        return;
    }
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(DESCRIPTOR_POOL_SIZE);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(DESCRIPTOR_POOL_SIZE);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = DESCRIPTOR_POOL_SIZE;
    if (vkCreateDescriptorPool(device->vkDevice, &poolInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS)
    {
        Logger::Fatal("failed to create descriptor pool!");
    }
}

void DescriptorResource::CleanupDescriptorPool(const std::shared_ptr<Device>& device)
{
    vkDestroyDescriptorPool(device->vkDevice, vkDescriptorPool, nullptr);
}

void DescriptorResource::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(Swapchain::MAX_FRAMES_IN_FLIGHT, vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    descriptorSetAllocateInfo.pSetLayouts = layouts.data();
    vkDescriptorSets.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_device->vkDevice, &descriptorSetAllocateInfo, vkDescriptorSets.data()) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate descriptor sets!");
    }
}


void DescriptorResource::Cleanup()
{
    if (!m_cleaned)
    {
        if (vkDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(m_device->vkDevice, vkDescriptorSetLayout, nullptr);
        }
        m_cleaned = true;
    }
}
