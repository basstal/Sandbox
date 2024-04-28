#include "pch.hpp"

#include "DescriptorSet.hpp"

#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include "FileSystem/Logger.hpp"
#include "Misc/Debug.hpp"
#include "Misc/TypeCasting.hpp"
#include "WriteDescriptorSet.hpp"


Sandbox::DescriptorSet::DescriptorSet(const std::shared_ptr<DescriptorPool>& descriptorPool, const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout)
{
    m_device         = descriptorPool->GetDevice();
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
    LOGI("VulkanRHI", "{}\n{}", PtrToHexString(vkDescriptorSet), GetCallStack())
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

            WriteDescriptorSet writeDescriptorSet{};
            auto&              vkWriteDescriptorSet = writeDescriptorSet.vkWriteDescriptorSet;
            vkWriteDescriptorSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            vkWriteDescriptorSet.dstBinding         = bindingIndex;
            vkWriteDescriptorSet.descriptorType     = out.descriptorType;
            vkWriteDescriptorSet.dstSet             = vkDescriptorSet;
            vkWriteDescriptorSet.pBufferInfo        = bufferInfos.empty() ? nullptr : bufferInfos.data();
            vkWriteDescriptorSet.dstArrayElement    = 0;
            vkWriteDescriptorSet.descriptorCount    = out.descriptorCount;
            writeDescriptorSets.push_back(writeDescriptorSet);
        }
        else
        {
            LOGW("VulkanRHI", "DescriptorSetLayout::TryGetLayoutBinding() failed with bindingIndex {} not found", std::to_string(bindingIndex))
        }
    }
}

void Sandbox::DescriptorSet::BindImageInfoMapping(const std::map<uint32_t, std::vector<VkDescriptorImageInfo>>& inImageInfoMapping,
                                                  const std::shared_ptr<DescriptorSetLayout>&                   descriptorSetLayout)
{
    // if (!writeDescriptorSets.empty() || !writeDescriptorSetsImage.empty())
    // {
    //     LOGW_OLD("DescriptorSet::Prepare() called more than once")
    //     return;
    // }

    VkDescriptorSetLayoutBinding out;

    m_imageInfoMapping = inImageInfoMapping;
    writeDescriptorSetsImage.clear();
    for (auto& [bindingIndex, imageInfos] : m_imageInfoMapping)
    {
        if (descriptorSetLayout->TryGetLayoutBinding(bindingIndex, out))
        {
            assert(imageInfos.size() == out.descriptorCount);
            WriteDescriptorSet writeDescriptorSet{};
            auto&              vkWriteDescriptorSet = writeDescriptorSet.vkWriteDescriptorSet;
            vkWriteDescriptorSet.sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            vkWriteDescriptorSet.dstBinding         = bindingIndex;
            vkWriteDescriptorSet.descriptorType     = out.descriptorType;
            vkWriteDescriptorSet.pImageInfo         = imageInfos.data();
            vkWriteDescriptorSet.dstSet             = vkDescriptorSet;
            vkWriteDescriptorSet.dstArrayElement    = 0;
            vkWriteDescriptorSet.descriptorCount    = out.descriptorCount;
            writeDescriptorSetsImage.push_back(writeDescriptorSet);
        }
        else
        {
            LOGW("VulkanRHI", "DescriptorSetLayout::TryGetLayoutBinding() failed with bindingIndex {} not found", std::to_string(bindingIndex))
        }
    }
}

void Sandbox::DescriptorSet::Update()
{
    if (m_cleaned)
    {
        LOGF("VulkanRHI", "Should not call here\n{}", GetCallStack())
    }
    std::vector<WriteDescriptorSet>   writeOperations;
    std::vector<VkWriteDescriptorSet> updateDescriptorSets;
    auto                              totalWriteCount = writeDescriptorSets.size() + writeDescriptorSetsImage.size();
    // LOGI("VulkanRHI", "totalWriteCount : {}", std::to_string(totalWriteCount))
    for (size_t i = 0; i < totalWriteCount; i++)
    {
        const WriteDescriptorSet& writeOperation = i < writeDescriptorSets.size() ? writeDescriptorSets[i] : writeDescriptorSetsImage[i - writeDescriptorSets.size()];
        auto                      it             = m_updatedBindings.find(writeOperation);
        // LOGD("VulkanRHI", "it == m_updatedBindings.end() : {}, {}", std::to_string(it == m_updatedBindings.end()),
        if (it == m_updatedBindings.end() || it->second != writeOperation.vkWriteDescriptorSet.dstBinding)
        {
            // LOGD("VulkanRHI", "writeOperation.dstSet : {}, binding : {}", PtrToHexString(writeOperation.vkWriteDescriptorSet.dstSet),
            //      std::to_string(writeOperation.vkWriteDescriptorSet.dstBinding))
            writeOperations.push_back(writeOperation);
            updateDescriptorSets.push_back(writeOperation.vkWriteDescriptorSet);
        }
    }
    // LOGI("VulkanRHI", "writeOperations.size() : {}", std::to_string(writeOperations.size()))
    if (!writeOperations.empty())
    {
        vkUpdateDescriptorSets(m_device->vkDevice, ToUInt32(updateDescriptorSets.size()), updateDescriptorSets.data(), 0, nullptr);
    }
    for (size_t i = 0; i < writeOperations.size(); ++i)
    {
        const WriteDescriptorSet& writeOperation = writeOperations[i];
        m_updatedBindings[writeOperation]        = writeOperation.vkWriteDescriptorSet.dstBinding;
    }
}
