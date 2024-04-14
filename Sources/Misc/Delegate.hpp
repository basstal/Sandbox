#pragma once
#include <functional>

namespace Sandbox
{
    class DelegateHandle
    {
    public:
        static DelegateHandle Null;

        DelegateHandle();

        DelegateHandle(const DelegateHandle& other);

        DelegateHandle(DelegateHandle&& other) noexcept;

        DelegateHandle& operator=(const DelegateHandle& other);

        DelegateHandle& operator=(DelegateHandle&& other) noexcept;

        uint64_t GetId() const;

        bool IsValid() const;

        // 重载相等运算符，用于比较两个DelegateHandle是否相同
        bool operator==(const DelegateHandle& other) const;

        bool operator<(const DelegateHandle& other) const;

    private:
        static uint64_t CurrentId;  // 用于生成唯一ID
        uint64_t        id;
    };

    template <typename AbstractData, typename... Args>
    class Delegate
    {
    public:
        // 构造函数，接受一个std::function
        Delegate(std::function<void(AbstractData, Args...)> f) : func(f), handle() {}

        // 用于有参数的委托函数
        void operator()(AbstractData data, Args... args) const
        {
            if (func)
            {
                func(data, std::forward<Args>(args)...);
            }
        }


        // 获取委托的句柄
        DelegateHandle GetHandle() const { return handle; }

    private:
        std::function<void(AbstractData, Args...)> func;
        DelegateHandle                             handle;
    };

    // 特化用于 AbstractData 为 void 的情况
    template <>
    class Delegate<void>
    {
    public:
        Delegate(std::function<void()> f) : func(f), handle() {}

        void operator()() const
        {
            if (func)
            {
                func();
            }
        }

        DelegateHandle GetHandle() const { return handle; }

    private:
        std::function<void()> func;
        DelegateHandle        handle;
    };
}  // namespace Sandbox

namespace std
{
    template <>
    struct hash<Sandbox::DelegateHandle>
    {
        std::size_t operator()(const Sandbox::DelegateHandle& handle) const noexcept
        {
            return std::hash<uint64_t>()(handle.GetId());
        }
    };
}
