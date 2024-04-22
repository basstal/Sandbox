#include "pch.hpp"

#include "DescriptorSetKey.hpp"

bool Sandbox::DescriptorSetKey::operator==(const DescriptorSetKey& other) const
{
    return descriptorPool->vkDescriptorPool == other.descriptorPool->vkDescriptorPool &&
        descriptorSetLayout->vkDescriptorSetLayout == other.descriptorSetLayout->vkDescriptorSetLayout && frameFlightIndex == other.frameFlightIndex;
}
