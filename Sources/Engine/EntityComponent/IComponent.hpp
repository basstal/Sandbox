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
        std::weak_ptr<GameObject> gameObject;

        virtual std::string GetDerivedClassName();

        virtual void Cleanup();
        Sandbox_IComponent_GENERATED
    };
}  // namespace Sandbox NAMESPACE()

File_IComponent_GENERATED
