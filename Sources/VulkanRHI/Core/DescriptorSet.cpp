#include "pch.hpp"

#include "DescriptorSet.hpp"

#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "VulkanRHI/Common/ResourceCaching.hpp"


Sandbox::DescriptorSet::DescriptorSet(const std::shared_ptr<Device>& device, const std::shared_ptr<DescriptorPool>& descriptorPool,
                                      const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout)
{
    m_device         = device;
    m_descriptorPool = descriptorPool;
    Allocate(descriptorPool, descriptorSetLayout);
    // Prepare(inBufferInfoMapping, inImageInfoMapping, descriptorSetLayout);
}

Sandbox::DescriptorSet::~DescriptorSet() { Cleanup(); }

void Sandbox::DescriptorSet::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    vkFreeDescriptorSets(m_device->vkDevice, m_descriptorPool->vkDescriptorPool, 1, &vkDescriptorSet);
    m_cleaned = true;
}

void Sandbox::DescriptorSet::Allocate(const std::shared_ptr<DescriptorPool>& descriptorPool, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = descriptorPool->vkDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = &descriptorSetLayout->vkDescriptorSetLayout;
    if (vkAllocateDescriptorSets(m_device->vkDevice, &descriptorSetAllocateInfo, &vkDescriptorSet) != VK_SUCCESS)
    {
        Logger::Fatal("failed to allocate descriptor sets!");
    }
    // LOGI("{}\n{}", PtrToHexString(vkDescriptorSet), GetCallStack())
}
void Sandbox::DescriptorSet::BindBufferInfoMapping(const BindingMap<VkDescriptorBufferInfo>&   inBufferInfoMapping,
                                                   const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout)
{
    m_bufferInfoMapping = inBufferInfoMapping;
    writeDescriptorSets.clear();
    auto                         maxUniformBuffer = m_device->GetMaxUniformBuffer();
    auto                         maxStorageBuffer = m_device->GetMaxStorageBuffer();
    VkDescriptorSetLayoutBinding out;
    for (auto& [bindingIndex, bufferInfos] : m_bufferInfoMapping)
    {
        if (descriptorSetLayout->TryGetLayoutBinding(bindingIndex, out))
        {
            assert(bufferInfos.size() == out.descriptorCount);
            auto bufferRangeLimit = bufferInfos[0].range;
            if (out.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER && bufferRangeLimit > maxUniformBuffer)
            {
                bufferRangeLimit = maxUniformBuffer;
            }
            else if (out.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER && bufferRangeLimit > maxStorageBuffer)
            {
                bufferRangeLimit = maxStorageBuffer;
            }
            bufferInfos[0].range = bufferRangeLimit;

            VkWriteDescriptorSet writeDescriptorSet{};
            writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstBinding      = bindingIndex;
            writeDescriptorSet.descriptorType  = out.descriptorType;
            writeDescriptorSet.dstSet          = vkDescriptorSet;
            writeDescriptorSet.pBufferInfo     = bufferInfos.data();
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorCount = out.descriptorCount;
            writeDescriptorSets.push_back(writeDescriptorSet);
        }
    }
}

void Sandbox::DescriptorSet::BindInfoMapping(const std::map<uint32_t, std::vector<VkDescriptorBufferInfo>>& inBufferInfoMapping,
                                             const std::map<uint32_t, std::vector<VkDescriptorImageInfo>>&  inImageInfoMapping,
                                             const std::shared_ptr<DescriptorSetLayout>&                    descriptorSetLayout)
{
    // if (!writeDescriptorSets.empty() || !writeDescriptorSetsImage.empty())
    // {
    //     LOGW_OLD("DescriptorSet::Prepare() called more than once")
    //     return;
    // }

    VkDescriptorSetLayoutBinding out;
    BindBufferInfoMapping(inBufferInfoMapping, descriptorSetLayout);

    m_imageInfoMapping = inImageInfoMapping;
    writeDescriptorSetsImage.clear();
    for (auto& [bindingIndex, imageInfos] : m_imageInfoMapping)
    {
        if (descriptorSetLayout->TryGetLayoutBinding(bindingIndex, out))
        {
            assert(imageInfos.size() == out.descriptorCount);
            VkWriteDescriptorSet writeDescriptorSet{};
            writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstBinding      = bindingIndex;
            writeDescriptorSet.descriptorType  = out.descriptorType;
            writeDescriptorSet.pImageInfo      = imageInfos.data();
            writeDescriptorSet.dstSet          = vkDescriptorSet;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorCount = out.descriptorCount;
            writeDescriptorSetsImage.push_back(writeDescriptorSet);
        }
    }
}

void Sandbox::DescriptorSet::Update()
{
    std::vector<VkWriteDescriptorSet> writeOperations;
    std::vector<uint64_t>             writeOperationsHash;
    auto                              totalWriteCount = writeDescriptorSets.size() + writeDescriptorSetsImage.size();
    for (size_t i = 0; i < totalWriteCount; i++)
    {
        const VkWriteDescriptorSet& writeOperation     = i < writeDescriptorSets.size() ? writeDescriptorSets[i] : writeDescriptorSetsImage[i - writeDescriptorSets.size()];
        size_t                      writeOperationHash = 0;
        HashParam(writeOperationHash, writeOperation);
        auto it = m_updatedBindings.find(writeOperation.dstBinding);
        if (it == m_updatedBindings.end() || it->second != writeOperationHash)
        {
            writeOperations.push_back(writeOperation);
            writeOperationsHash.push_back(writeOperationHash);
        }
    }
    if (!writeOperations.empty())
    {
        vkUpdateDescriptorSets(m_device->vkDevice, static_cast<uint32_t>(writeOperations.size()), writeOperations.data(), 0, nullptr);
    }
    for (size_t i = 0; i < writeOperations.size(); ++i)
    {
        const VkWriteDescriptorSet& writeOperation     = writeOperations[i];
        const uint64_t&             writeOperationHash = writeOperationsHash[i];
        m_updatedBindings[writeOperation.dstBinding]   = writeOperationHash;
    }
}
