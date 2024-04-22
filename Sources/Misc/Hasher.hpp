#pragma once

namespace Sandbox
{

    template <typename T>
    void HashCombined(uint64_t& hash, const T& param)
    {
        std::hash<T> hasher;
        // 这里使用的是0x9e3779b9，它是黄金分割比的倒数与2^32的乘积的近似值，有助于分散哈希值。
        hash ^= hasher(param) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }


    /**
     * NOTE:除了 hash 以外 类型 T 还需要等价性函数来确保不会因为 hash 冲突导致获取到字段值不一致的对象
     * @tparam T
     * @param hash
     * @param param
     */
    template <typename T>
    void HashParam(uint64_t& hash, const T& param)
    {
        HashCombined(hash, param);
    }
}  // namespace Sandbox
