#include "Delegate.hpp"

// 初始化静态成员变量
uint64_t DelegateHandle::currentId = 0;
DelegateHandle DelegateHandle::Null = DelegateHandle();

DelegateHandle::DelegateHandle(): id(currentId++)
{
}

uint64_t DelegateHandle::GetId() const
{
	return id;
}

bool DelegateHandle::IsValid() const
{
	return *this != Null;
}

bool DelegateHandle::operator==(const DelegateHandle& other) const
{
	return id == other.id;
}
