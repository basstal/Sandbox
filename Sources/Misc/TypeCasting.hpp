#pragma once
#include <cstdint>

namespace Sandbox {
inline int32_t ToInt32(float inValue)
{
    return static_cast<int32_t>(inValue);
}
inline uint32_t ToUInt32(float inValue)
{
    return static_cast<uint32_t>(inValue);
}

inline uint32_t ToUInt32(size_t inValue)
{
    return static_cast<uint32_t>(inValue);
}

inline float ToFloat(uint32_t inValue)
{
    return static_cast<float>(inValue);
}
}
