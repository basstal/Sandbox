#pragma once
#include <memory>

#include "Serialization/ISerializable.hpp"

namespace Sandbox
{
    class GameObject;

    class IComponent
    {
    public:
        std::weak_ptr<GameObject> gameObject;

        virtual void Cleanup() = 0;
    };
}  // namespace Sandbox
