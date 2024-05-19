#include "pch.hpp"

#include "VertexInputStateBindingModifier.hpp"

#include "Misc/TypeCasting.hpp"

std::vector<VkVertexInputBindingDescription> Sandbox::VertexInputStateBindingModifier::CreateBindings()
{
    assert(strides.size() == inputRates.size());
    std::vector<VkVertexInputBindingDescription> bindings;
    for (size_t i = 0; i < strides.size(); ++i)
    {
        VkVertexInputBindingDescription binding = {};
        binding.binding                         = ToUInt32(i);
        binding.stride                          = strides[i];
        binding.inputRate                       = inputRates[i];
        bindings.push_back(binding);
    }
    return bindings;
}
