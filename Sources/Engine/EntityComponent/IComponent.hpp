#pragma once
#include <memory>

#include "Serialization/ISerializable.hpp"
#include "Generated/IComponent.rfkh.h"

namespace Sandbox NAMESPACE()
{
    class GameObject;

    /**
     * \brief TODO:不能直接派生自 ISerializable 因为这个类要用做统一接口，或者考虑把 ISerializable 的模板参数去掉
     * 这个类用 GetDerivedClassId 获得派生类的反射 id
     */
    class CLASS() IComponent
    {
    public:
        virtual ~IComponent() = default;


        std::weak_ptr<GameObject> gameObject;

        virtual std::string GetDerivedClassName() = 0;

        virtual void Cleanup();
        Sandbox_IComponent_GENERATED
    };
}  // namespace Sandbox

File_IComponent_GENERATED
