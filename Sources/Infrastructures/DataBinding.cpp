#include "DataBinding.hpp"

IDataBinding::IDataBinding(std::string InName): Name(InName)
{
}

std::map<std::string, std::shared_ptr<IDataBinding>> DataBinding::DataBindingMap = std::map<std::string, std::shared_ptr<IDataBinding>>();

std::shared_ptr<IDataBinding> DataBinding::Get(std::string InName)
{
	auto ValueTypePtr = DataBinding::DataBindingMap.find(InName);
	if (ValueTypePtr != DataBinding::DataBindingMap.end())
	{
		return ValueTypePtr->second;
	}
	return nullptr;
}

bool DataBinding::UnBind(std::string InName, DelegateHandle InDelegateHandle)
{
	auto DataBinding = DataBinding::Get(InName);
	return DataBinding->UnBind(InDelegateHandle);
}

bool DataBinding::Delete(std::string InName)
{
	auto ValueTypePtr = DataBinding::DataBindingMap.find(InName);
	if (ValueTypePtr != DataBinding::DataBindingMap.end())
	{
		std::shared_ptr<IDataBinding> DataBindingRef = ValueTypePtr->second;
		DataBindingRef->Release();
		DataBinding::DataBindingMap.erase(InName);
		return true;
	}
	return false;
}

void DataBinding::Trigger(std::string InName, const DelegateHandle& InDelegateHandle)
{
	if (auto DataBinding = Get(InName))
	{
		DataBinding->Trigger(InDelegateHandle);
	}
}
