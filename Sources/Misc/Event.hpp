#pragma once
#include <map>
#include <memory>

#include "Delegate.hpp"

namespace Sandbox
{
    template <typename... Args>
    class Event
    {
    public:
        std::map<DelegateHandle, Delegate<Args...>> registeredDelegates;

        DelegateHandle Bind(const Delegate<Args...>& delegate)
        {
            registeredDelegates.insert(std::make_pair(delegate.GetHandle(), delegate));
            return delegate.GetHandle();
        }

        DelegateHandle Bind(void (*Func)(Args...))
        {
            Delegate<Args...> delegate(Func);
            registeredDelegates.insert(std::make_pair(delegate.GetHandle(), delegate));
            return delegate.GetHandle();
        }

        // 使用std::function绑定
        DelegateHandle Bind(const std::function<void(Args...)>& func)
        {
            Delegate<Args...> delegate(func);
            registeredDelegates.insert(std::make_pair(delegate.GetHandle(), delegate));
            return delegate.GetHandle();
        }

        template <typename Instance, void (Instance::*Func)(Args...)>
        DelegateHandle BindMember(Instance* obj)
        {
            Delegate<Args...> boundDelegate(
                [=](Args... args)
                {
                    // TODO:这里没验证 obj 是否还有效
                    (*obj.*Func)(std::forward<Args>(args)...);
                });
            registeredDelegates.insert(std::make_pair(boundDelegate.GetHandle(), boundDelegate));
            return boundDelegate.GetHandle();
        }

        template <typename Instance, void (Instance::*Func)(Args...)>
        DelegateHandle BindMember(std::shared_ptr<Instance> sharedPtr)
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
            registeredDelegates.insert(std::make_pair(boundDelegate.GetHandle(), boundDelegate));
            return boundDelegate.GetHandle();
        }

        void Unbind(DelegateHandle handle)
        {
            if (registeredDelegates.contains(handle))
            {
                registeredDelegates.erase(handle);
            }
        }

        void Trigger(Args... args)
        {
            // TODO:这里不能用完美转发，会将导致多次使用同一个右值时，只有第一次的有效，后续右值为空
            for (auto it = registeredDelegates.cbegin(); it != registeredDelegates.cend(); ++it)
            {
                it->second(args...);
            }
        }
    };


    template <>
    class Event<void>
    {
    public:
        std::map<DelegateHandle, Delegate<void>> registeredDelegates;

        DelegateHandle Bind(const Delegate<void>& delegate)
        {
            registeredDelegates.insert(std::make_pair(delegate.GetHandle(), delegate));
            return delegate.GetHandle();
        }

        template <typename Instance, void (Instance::*Func)()>
        DelegateHandle BindMember(Instance* obj)
        {
            Delegate<void> boundDelegate(std::bind(Func, obj));
            registeredDelegates.insert(std::make_pair(boundDelegate.GetHandle(), boundDelegate));
            return boundDelegate.GetHandle();
        }

        template <typename Instance, void (Instance::*Func)()>
        DelegateHandle BindMember(std::weak_ptr<Instance> weakObj)
        {
            Delegate<void> boundDelegate(
                [weakObj]()
                {
                    auto obj = weakObj.lock();
                    if (obj)
                    {
                        (*obj.*Func)();
                    }
                });
            registeredDelegates.insert(std::make_pair(boundDelegate.GetHandle(), boundDelegate));
            return boundDelegate.GetHandle();
        }

        void UnBind(DelegateHandle handle)
        {
            if (registeredDelegates.contains(handle))
            {
                registeredDelegates.erase(handle);
            }
        }

        void Trigger()
        {
            for (auto it = registeredDelegates.cbegin(); it != registeredDelegates.cend(); ++it)
			{
				it->second();
			}
        }
    };
}
