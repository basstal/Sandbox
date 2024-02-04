#pragma once

#include <functional>


class DelegateHandle
{
private:
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
private:
	std::function<void(AbstractData)> func;
	DelegateHandle handle;

public:
	// 构造函数，接受一个std::function
	Delegate(std::function<void(AbstractData)> f) : func(f), handle()
	{
	}

	// 调用委托函数
	void operator()(AbstractData data) const
	{
		if (func)
		{
			// 检查函数是否不为null
			func(data);
		}
	}

	// 获取委托的句柄
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
