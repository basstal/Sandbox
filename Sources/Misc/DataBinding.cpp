#include "pch.hpp"

#include "DataBinding.hpp"

Sandbox::IDataBinding::IDataBinding(std::string InName) : Name(InName) {}

Sandbox::DelegateHandle Sandbox::TDataBinding<void>::Bind(DelegateType InDelegate)
{
    DelegateHandle DelegateHandle = InDelegate.GetHandle();
    if (!DelegateHandleSet.contains(DelegateHandle))
    {
        Slots.push_back(InDelegate);
        DelegateHandleSet.emplace(DelegateHandle);
    }
    return DelegateHandle;
}
Sandbox::DelegateHandle Sandbox::TDataBinding<void>::Bind(const std::function<void()>& func) { return Bind(DelegateType(func)); }

bool Sandbox::TDataBinding<void>::UnBind(DelegateHandle InDelegateHandle)
{
    if (DelegateHandleSet.contains(InDelegateHandle))
    {
        DelegateHandleSet.erase(InDelegateHandle);
        for (auto Item = Slots.begin(); Item != Slots.end(); ++Item)
        {
            if (static_cast<DelegateType>(*Item).GetHandle() == InDelegateHandle)
            {
                Slots.erase(Item);
                return true;
            }
        }
        std::cerr << "UnBind in DelegateHandleSet, but not in Slots??\n";
    }
    return false;
}

void Sandbox::TDataBinding<void>::Trigger(const DelegateHandle& InDelegateHandle)
{
    bool bHasInDelegateHandle = InDelegateHandle.IsValid();
    for (DelegateType& Slot : Slots)
    {
        if (!bHasInDelegateHandle || (bHasInDelegateHandle && Slot.GetHandle() == InDelegateHandle))
        {
            Slot();
        }
    }
}

void Sandbox::TDataBinding<void>::Release() {}

Sandbox::TDataBinding<void>::TDataBinding(std::string InName) : IDataBinding(InName) {}


auto Sandbox::DataBinding::Create(std::string InName) -> std::shared_ptr<TDataBinding<void>>
{
    auto entry = DataBinding::DataBindingMap.find(InName);
    if (entry != DataBinding::DataBindingMap.end())
    {
        try
        {
            return std::any_cast<std::shared_ptr<TDataBinding<void>>>(entry->second);
        }
        catch (const std::bad_any_cast&)
        {
            Logger::Fatal("Failed to cast binding to the requested type for name: " + InName);
        }
    }
    std::shared_ptr<TDataBinding<void>> CreatedDataBinding = std::make_shared<TDataBinding<void>>(InName);
    DataBinding::DataBindingMap.emplace(InName, CreatedDataBinding);
    return CreatedDataBinding;
}

void Sandbox::DataBinding::Trigger(std::string InName, const DelegateHandle& InDelegateHandle) { Logger::Error("The method or operation is not implemented."); }

std::map<std::string, std::any> Sandbox::DataBinding::DataBindingMap = std::map<std::string, std::any>();
