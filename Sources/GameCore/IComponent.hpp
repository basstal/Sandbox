#pragma once
#include <memory>

class GameObject;

class IComponent
{
public:
    std::shared_ptr<GameObject> gameObject;

    virtual void Update() = 0;

    virtual ~IComponent() = default;

    virtual void Cleanup() = 0;
};

// 使用 std::is_base_of 和静态断言定义概念
template <typename T>
concept DerivedFromIComponent = std::is_base_of_v<IComponent, T>;
