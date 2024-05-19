#pragma once

namespace Sandbox
{
    class VertexInputStateBindingModifier
    {
    public:
        std::vector<uint32_t>                        strides;
        std::vector<VkVertexInputRate>               inputRates;
        std::vector<VkVertexInputBindingDescription> CreateBindings();
    };
}  // namespace Sandbox
