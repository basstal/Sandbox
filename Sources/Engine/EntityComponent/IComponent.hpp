#pragma once
#include <memory>

#include "Generated/IComponent.rfkh.h"
#include "Serialization/ISerializable.hpp"

namespace Sandbox NAMESPACE()
{
    class GameObject;

    class CLASS() IComponent : public ISerializable
    {
    public:
        static Event<const std::shared_ptr<IComponent>&> onComponentCreate;

        std::weak_ptr<GameObject> gameObject;

        virtual std::string GetDerivedClassName();

        virtual const rfk::Class* GetDerivedClass();

        virtual void Cleanup();
        Sandbox_IComponent_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_IComponent_GENERATED
