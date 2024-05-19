#pragma once

#include <memory>
#include <vector>

#include "Misc/Hasher.hpp"

namespace Sandbox
{

    // struct DescriptorSetPreset
    // {
    //     std::set<uint32_t> filteredBindingIndex;  // 假设这个是成员变量
    // };

    class DescriptorSetsPreset
    {
    public:
        DescriptorSetsPreset(std::vector<std::set<uint32_t>> presets) : count(presets.size()), m_presets(std::move(presets)) {}

        // 重载 [] 操作符
        std::set<uint32_t>& operator[](size_t index) { return m_presets[index]; }

        const std::set<uint32_t>& operator[](size_t index) const { return m_presets[index]; }

        size_t count;  // 公开的计数变量

    private:
        std::vector<std::set<uint32_t>> m_presets;
    };
}  // namespace Sandbox


namespace std
{
    template <>
    struct hash<Sandbox::DescriptorSetsPreset>
    {
        size_t operator()(const Sandbox::DescriptorSetsPreset& descriptorSetsPreset) const noexcept
        {
            size_t result = 0;
            for (size_t i = 0; i < descriptorSetsPreset.count; ++i)
            {
                for (auto& bindingIndex : descriptorSetsPreset[i])
                {
                    Sandbox::HashCombined(result, bindingIndex);
                }
            }
            return result;
        }
    };
}