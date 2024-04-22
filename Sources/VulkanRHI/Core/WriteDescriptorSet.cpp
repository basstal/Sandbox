#include "pch.hpp"

#include "WriteDescriptorSet.hpp"

bool Sandbox::WriteDescriptorSet::operator==(const WriteDescriptorSet& other) const
{
    return vkWriteDescriptorSet.dstSet == other.vkWriteDescriptorSet.dstSet && vkWriteDescriptorSet.dstBinding == other.vkWriteDescriptorSet.dstBinding &&
        vkWriteDescriptorSet.dstArrayElement == other.vkWriteDescriptorSet.dstArrayElement &&
        vkWriteDescriptorSet.descriptorCount == other.vkWriteDescriptorSet.descriptorCount && vkWriteDescriptorSet.descriptorType == other.vkWriteDescriptorSet.descriptorType &&
        vkWriteDescriptorSet.pImageInfo == other.vkWriteDescriptorSet.pImageInfo && vkWriteDescriptorSet.pBufferInfo == other.vkWriteDescriptorSet.pBufferInfo &&
        vkWriteDescriptorSet.pTexelBufferView == other.vkWriteDescriptorSet.pTexelBufferView;  // 这里仅比较指针是否足够，可能还需要比较指针指向数组内的数据？
    
        }