#pragma once
#include "Delegate.hpp"

namespace Sandbox
{
    template <typename... Args>
    class PriorityEvent
    {
    public:
        // 更改为 multimap，键为负的优先级，以保证高优先级在前
        std::multimap<int, Delegate<Args...>, std::greater<>> registeredDelegates;

        // 绑定函数，并指定优先级
        DelegateHandle Bind(const Delegate<Args...>& delegate, int priority)
        {
            registeredDelegates.insert({priority, delegate});
            return delegate.GetHandle();
        }

        DelegateHandle Bind(void (*Func)(Args...), int priority)
        {
            Delegate<Args...> delegate(Func);
            registeredDelegates.insert({priority, delegate});
            return delegate.GetHandle();
        }

        DelegateHandle Bind(const std::function<void(Args...)>& func, int priority)
        {
            Delegate<Args...> delegate(func);
            registeredDelegates.insert({priority, delegate});
            return delegate.GetHandle();
        }

        template <typename Instance, void (Instance::*Func)(Args...)>
        DelegateHandle BindMember(Instance* obj, int priority)
        {
            Delegate<Args...> boundDelegate([=](Args... args) { (*obj.*Func)(std::forward<Args>(args)...); });
            registeredDelegates.insert({priority, boundDelegate});
            return boundDelegate.GetHandle();
        }

        template <typename Instance, void (Instance::*Func)(Args...)>
        DelegateHandle BindMember(std::shared_ptr<Instance> sharedPtr, int priority)
        {
            auto              weakPtr = std::weak_ptr<Instance>(sharedPtr);
            Delegate<Args...> boundDelegate(
                [weakPtr](Args... args)
                {
                    auto obj = weakPtr.lock();
                    if (obj)
                    {
                        (*obj.*Func)(std::forward<Args>(args)...);
                    }
                });
            registeredDelegates.insert({priority, boundDelegate});
            return boundDelegate.GetHandle();
        }

        // 删除一个绑定的代理
        void Unbind(DelegateHandle handle)
        {
            for (auto it = registeredDelegates.begin(); it != registeredDelegates.end(); ++it)
            {
                if (it->second.GetHandle() == handle)
                {
                    registeredDelegates.erase(it);
                    return;  // 假设每个 handle 唯一，找到即删除
                }
            }
        }

        // 触发所有事件，按优先级排序
        void Trigger(Args... args)
        {
            for (auto& [priority, delegate] : registeredDelegates)
            {
                // TODO:这里不能用完美转发，会将导致多次使用同一个右值时，只有第一次的有效，后续右值为空
                delegate(args...);
            }
        }
        void Cleanup() { registeredDelegates.clear(); }
    };
}  // namespace Sandbox
