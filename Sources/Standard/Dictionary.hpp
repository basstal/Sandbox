#pragma once

namespace Sandbox
{
    template <typename TKey, typename TValue>
    using Dictionary = std::unordered_map<TKey, TValue>;
}  // namespace Sandbox
