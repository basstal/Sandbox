#pragma once
#include <memory>

#include "Generated/IComponent.rfkh.h"
#include "Serialization/ISerializable.hpp"

namespace Sandbox NAMESPACE()
{
    class GameObject;
    class Transform;

    class CLASS() IComponent : public ISerializable
    {
    public:
        /**
         * \brief 绑定在这里的事件会在 构造函数 => 反序列化所有字段 以后触发
         */
        static Event<const std::shared_ptr<IComponent>&> onComponentCreate;

        std::weak_ptr<GameObject> gameObject;

        std::weak_ptr<Transform> transform;

        virtual std::string GetDerivedClassName();

        virtual const rfk::Class* GetDerivedClass();

        virtual void Cleanup();
        Sandbox_IComponent_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_IComponent_GENERATED
