#pragma once
#include <memory>
#include <vector>

class IEditor
{
private:
	static int m_nextId; // 静态变量用于生成唯一ID
	int m_id; // 每个实例的唯一ID

public:
	virtual ~IEditor() = default;
	static std::vector<std::shared_ptr<IEditor>> registeredEditors;

	// 在构造函数中分配唯一ID
	IEditor() : m_id(m_nextId++)
	{
	}

	void Register(std::shared_ptr<IEditor> editor);

	void Unregister();
	// 重载==操作符来比较两个IEditor实例是否相等
	bool operator==(const IEditor& other) const
	{
		return m_id == other.m_id;
	}

	// 如果需要，也可以重载!=操作符
	bool operator!=(const IEditor& other) const
	{
		return !(*this == other);
	}

	virtual void DrawFrame() = 0;
};
