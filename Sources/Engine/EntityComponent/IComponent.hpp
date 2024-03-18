#pragma once
#include <memory>


namespace Sandbox
{
    class GameObject;

    class IComponent
    {
    public:
        std::weak_ptr<GameObject> gameObject;

        virtual ~IComponent() = default;

        virtual void Cleanup() = 0;
    };
}
