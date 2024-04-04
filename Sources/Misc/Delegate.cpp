#include "pch.hpp"

#include "Delegate.hpp"

// 初始化静态成员变量
uint64_t                Sandbox::DelegateHandle::currentId = 0;
Sandbox::DelegateHandle Sandbox::DelegateHandle::Null      = DelegateHandle();

Sandbox::DelegateHandle::DelegateHandle() : id(currentId++) {}

Sandbox::DelegateHandle::DelegateHandle(const DelegateHandle& other) { id = other.id; }

Sandbox::DelegateHandle::DelegateHandle(DelegateHandle&& other) noexcept { id = other.id; }

Sandbox::DelegateHandle& Sandbox::DelegateHandle::operator=(const DelegateHandle& other)
{
    id = other.id;
    return *this;
}

Sandbox::DelegateHandle& Sandbox::DelegateHandle::operator=(DelegateHandle&& other) noexcept
{
    id = other.id;
    return *this;
}

uint64_t Sandbox::DelegateHandle::GetId() const { return id; }

bool Sandbox::DelegateHandle::IsValid() const { return *this != Null; }

bool Sandbox::DelegateHandle::operator==(const DelegateHandle& other) const { return id == other.id; }

bool Sandbox::DelegateHandle::operator<(const DelegateHandle& other) const { return id < other.id;
}
