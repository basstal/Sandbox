#pragma once

namespace Sandbox
{
    template <typename TKey, typename TValue>
    using Dictionary = std::unordered_map<TKey, TValue>;

    template <typename TKey>
    using Set = std::unordered_set<TKey>;
}  // namespace Sandbox
