#pragma once
#include <ios>
#include <sstream>
#include <string>

namespace Sandbox
{
    std::string GetCallStack();

    template <typename T>
    std::string PtrToHexString(T inT)
    {
        std::stringstream ss;
        ss << "0x" << std::hex << reinterpret_cast<uintptr_t>(inT);  // 使用stringstream和hex格式化
        return ss.str();
    }
}
