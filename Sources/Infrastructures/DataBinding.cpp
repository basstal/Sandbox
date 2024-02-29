#include "DataBinding.hpp"

IDataBinding::IDataBinding(std::string InName):
    Name(InName)
{
}

TDataBinding<void>::~TDataBinding()
{

}

DelegateHandle TDataBinding<void>::Bind(DelegateType InDelegate)
{
    DelegateHandle DelegateHandle = InDelegate.GetHandle();
    if (!DelegateHandleSet.contains(DelegateHandle))
    {
        Slots.push_back(InDelegate);
        DelegateHandleSet.emplace(DelegateHandle);
    }
    return DelegateHandle;
}

bool TDataBinding<void>::UnBind(DelegateHandle InDelegateHandle)
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

void TDataBinding<void>::Trigger(const DelegateHandle& InDelegateHandle)
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

void TDataBinding<void>::Release()
{
}

TDataBinding<void>::TDataBinding(std::string InName):
    IDataBinding(InName)
{
}


auto DataBinding::Create(std::string InName) -> std::shared_ptr<TDataBinding<void>>
{
    auto entry = DataBinding::DataBindingMap.find(InName);
    if (entry != DataBinding::DataBindingMap.end())
    {
        try
        {
            return std::any_cast<std::shared_ptr<TDataBinding<void>>>(entry->second);
        }
        catch (const std::bad_any_cast& _) // NOLINT(clang-diagnostic-unused-exception-parameter)
        {
            Logger::Fatal("Failed to cast binding to the requested type for name: " + InName);
        }
    }
    std::shared_ptr<TDataBinding<void>> CreatedDataBinding = std::make_shared<TDataBinding<void>>(InName);
    DataBinding::DataBindingMap.emplace(InName, CreatedDataBinding);
    return CreatedDataBinding;
}

void DataBinding::Trigger(std::string InName, const DelegateHandle& InDelegateHandle)
{
    Logger::Error("The method or operation is not implemented.");
}

std::map<std::string, std::any> DataBinding::DataBindingMap = std::map<std::string, std::any>();

// std::shared_ptr<IDataBinding> DataBinding::Get(std::string InName)
// {
// 	auto ValueTypePtr = DataBinding::DataBindingMap.find(InName);
// 	if (ValueTypePtr != DataBinding::DataBindingMap.end())
// 	{
// 		return ValueTypePtr->second;
// 	}
// 	return nullptr;
// }

// bool DataBinding::UnBind(std::string InName, DelegateHandle InDelegateHandle)
// {
// 	auto DataBinding = DataBinding::Get(InName);
// 	return DataBinding->UnBind(InDelegateHandle);
// }

// bool DataBinding::Delete(std::string InName)
// {
// 	auto ValueTypePtr = DataBinding::DataBindingMap.find(InName);
// 	if (ValueTypePtr != DataBinding::DataBindingMap.end())
// 	{
// 		std::shared_ptr<IDataBinding> DataBindingRef = ValueTypePtr->second;
// 		DataBindingRef->Release();
// 		DataBinding::DataBindingMap.erase(InName);
// 		return true;
// 	}
// 	return false;
// }

// void DataBinding::Trigger(std::string InName, const DelegateHandle& InDelegateHandle)
// {
// 	if (auto DataBinding = Get(InName))
// 	{
// 		DataBinding->Trigger(InDelegateHandle);
// 	}
// }
