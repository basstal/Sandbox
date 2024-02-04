#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

#include "Delegate.hpp"

class IDataBinding
{
public:
	virtual void Trigger(const DelegateHandle& InDelegateHandle = DelegateHandle::Null) = 0;
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	virtual void Release() = 0;
	virtual bool UnBind(DelegateHandle InDelegateHandle) = 0;
	virtual ~IDataBinding() = default;

protected:
	IDataBinding(std::string InName);

	// 保存所有存在于Slots中的Handle
	std::unordered_set<DelegateHandle> DelegateHandleSet;
	// 数据绑定的名称
	std::string Name;
};


template <typename AbstractData>
class TDataBinding : public IDataBinding
{
	friend class DataBinding;

public:
	virtual ~TDataBinding() override;

	typedef Delegate<AbstractData> DelegateType; // 用于创建绑定函数的TDelegate签名
	/**
	 * 添加数据的绑定函数
	 *
	 * @param InDelegate 外部构造的绑定函数，推荐使用相关宏构造：CreateLambdaBind、CreateRawBind
	 * @return 绑定函数FDelegate的Handle，用于解绑时作为参数传入
	 */
	DelegateHandle Bind(DelegateType InDelegate);
	/**
	 * 获得数据原型
	 *
	 * @return 内部的数据原型
	 */
	AbstractData GetData();
	/**
	 * 设置数据原型，触发绑定函数，如果与内部的数据原型相同则不触发
	 *
	 * @param InData 待设置的数据原型
	 */
	void SetData(AbstractData InData);
	//~Begin IDataBinding interface
	virtual bool UnBind(DelegateHandle InDelegateHandle) override;
	virtual void Trigger(const DelegateHandle& InDelegateHandle = DelegateHandle::Null) override;
	virtual void Release() override;
	//~End IDataBinding interface
	TDataBinding(std::string InName, AbstractData InData);

protected:
	// 数据原型
	AbstractData Data;
	// 保存绑定函数
	std::vector<DelegateType> Slots;
};


class DataBinding
{
public:
	/**
	 * 获得指定名称的数据绑定，
	 * 推荐UObject及其子类模板加裸指针，其他类型结构模板加TSharedPtr，基本数据类型可直接使用
	 *
	 * @param InName 数据绑定对象自定义名称，同时查找用
	 * @return 数据绑定
	 */
	static std::shared_ptr<IDataBinding> Get(std::string InName);

	/**
	 * 给指定数据绑定解绑函数
	 * @param InName
	 * @param InDelegateHandle
	 * @return
	 */
	static bool UnBind(std::string InName, DelegateHandle InDelegateHandle);
	/**
	 * 构造指定名称的数据绑定，并传入数据原型，
	 * 推荐UObject及其子类模板加裸指针，其他类型结构模板加TSharedPtr，基本数据类型可直接使用
	 *
	 * @param InName 数据绑定对象自定义名称，同时查找用
	 * @param InData 数据原型
	 * @return 数据绑定
	 */
	// template <typename AbstractData>
	// static std::shared_ptr<TDataBindingSP<AbstractData>> CreateSP(std::string InName, std::shared_ptr<AbstractData> InData);
	//
	// template <typename AbstractData>
	// static std::shared_ptr<TDataBindingUObject<AbstractData>> CreateUObject(std::string InName, AbstractData* InData);

	template <typename AbstractData>
	static std::shared_ptr<TDataBinding<AbstractData>> Create(std::string InName, AbstractData InData);

	/**
	 * 清除指定名称的数据绑定
	 *
	 * @param InName 数据绑定对象自定义名称
	 * @return 是否成功删除
	 */
	static bool Delete(std::string InName);

	/**
	 * 触发指定名称的数据绑定的所有绑定函数
	 *
	 * @param InName 指定名称的数据绑定
	 * @param InDelegateHandle （可选的）若传入绑定函数的 Handle ，以便只调用这个已绑定的函数
	 */
	static void Trigger(std::string InName, const DelegateHandle& InDelegateHandle = DelegateHandle());

protected:
	static std::map<std::string, std::shared_ptr<IDataBinding>> DataBindingMap; // 名称到数据绑定的映射
};


template <typename AbstractData>
TDataBinding<AbstractData>::TDataBinding(std::string InName, AbstractData InData)
	: IDataBinding(InName),
	  Data(InData)
{
}

template <typename AbstractData>
TDataBinding<AbstractData>::~TDataBinding()
{
	TDataBinding<AbstractData>::Release();
}


template <typename AbstractData>
DelegateHandle TDataBinding<AbstractData>::Bind(DelegateType InDelegate)
{
	DelegateHandle DelegateHandle = InDelegate.GetHandle();
	if (!DelegateHandleSet.contains(DelegateHandle))
	{
		Slots.push_back(InDelegate);
		DelegateHandleSet.emplace(DelegateHandle);
	}
	return DelegateHandle;
}


template <typename AbstractData>
bool TDataBinding<AbstractData>::UnBind(DelegateHandle InDelegateHandle)
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


template <typename AbstractData>
void TDataBinding<AbstractData>::Trigger(const DelegateHandle& InDelegateHandle)
{
	bool bHasInDelegateHandle = InDelegateHandle.IsValid();
	for (DelegateType& Slot : Slots)
	{
		if (!bHasInDelegateHandle || (bHasInDelegateHandle && Slot.GetHandle() == InDelegateHandle))
		{
			Slot(Data);
		}
	}
}

template <typename AbstractData>
AbstractData TDataBinding<AbstractData>::GetData()
{
	return Data;
}

template <typename AbstractData>
void TDataBinding<AbstractData>::SetData(AbstractData InData)
{
	Data = InData;
	Trigger();
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Release()
{
	DelegateHandleSet.clear();
	Slots.clear();
}

template <typename AbstractData>
std::shared_ptr<TDataBinding<AbstractData>> DataBinding::Create(std::string InName, AbstractData InData)
{
	auto entry = DataBinding::DataBindingMap.find(InName);
	if (entry != DataBinding::DataBindingMap.end())
	{
		return std::dynamic_pointer_cast<TDataBinding<AbstractData>>(entry->second);
	}
	std::shared_ptr<TDataBinding<AbstractData>> CreatedDataBinding = std::make_shared<TDataBinding<AbstractData>>(InName, InData);
	DataBinding::DataBindingMap.emplace(InName, CreatedDataBinding);
	return CreatedDataBinding;
}
