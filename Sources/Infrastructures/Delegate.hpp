#pragma once

#include <functional>

#include "TypeTraits.hpp"

class DelegateHandle
{
    static uint64_t currentId; // 用于生成唯一ID
    uint64_t id;

public:
    static DelegateHandle Null;

    DelegateHandle();

    uint64_t GetId() const;

    bool IsValid() const;

    // 重载相等运算符，用于比较两个DelegateHandle是否相同
    bool operator==(const DelegateHandle& other) const;
};

// 定义Delegate类型，它可以封装任何返回void并接受AbstractData类型参数的函数
template <typename AbstractData>
class Delegate
{
    std::function<void(AbstractData)> func;
    DelegateHandle handle;

public:
    // 构造函数，接受一个std::function
    Delegate(std::function<void(AbstractData)> f) :
        func(f), handle()
    {
    }

    // 用于有参数的委托函数
    void operator()(AbstractData data) const
    {
        if (func)
        {
            func(data);
        }
    }


    // 获取委托的句柄
    DelegateHandle GetHandle() const
    {
        return handle;
    }
};

// 特化用于 AbstractData 为 void 的情况
template <>
class Delegate<void>
{
    std::function<void()> func;
    DelegateHandle handle;

public:
    Delegate(std::function<void()> f) :
        func(f), handle()
    {
    }

    void operator()() const
    {
        if (func)
        {
            func();
        }
    }

    DelegateHandle GetHandle() const
    {
        return handle;
    }
};

namespace std
{
    template <>
    struct hash<DelegateHandle>
    {
        std::size_t operator()(const DelegateHandle& handle) const noexcept
        {
            return std::hash<uint64_t>()(handle.GetId());
        }
    };
}
