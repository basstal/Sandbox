#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Components/Transform.hpp"
#include "Generated/GameObject.rfkh.h"
#include "Misc/String.hpp"
#include "Serialization/ISerializable.hpp"
#include "Serialization/SharedPtr.hpp"

namespace Sandbox NAMESPACE()
{
    class IComponent;

    template <typename T>
    concept DerivedFromIComponent = std::is_base_of_v<IComponent, T>;

    class CLASS() GameObject : public ISerializable<GameObject>
    {
    public:
        FIELD()
        SharedPtr<Transform> transform;
        FIELD()
        String name;

        std::vector<std::shared_ptr<GameObject>> children;

        GameObject();

        ~GameObject();

        void Cleanup();

        // TODO:改为泛型内部构造
        void AddComponent(std::shared_ptr<IComponent> component);

        template <DerivedFromIComponent T>
        std::shared_ptr<T> AddComponent();

        template <DerivedFromIComponent T>
        std::shared_ptr<T> GetComponent();

        std::vector<std::shared_ptr<IComponent>> GetComponents();

    private:
        bool m_cleaned = false;

        std::vector<std::shared_ptr<IComponent>> m_components;

        Sandbox_GameObject_GENERATED
    };

    template <DerivedFromIComponent T>
    std::shared_ptr<T> GameObject::GetComponent()
    {
        for (auto& component : m_components)
        {
            auto asT = std::dynamic_pointer_cast<T>(component);
            if (asT)
            {
                return asT;
            }
        }
        return nullptr;
    }

    template <DerivedFromIComponent T>
    std::shared_ptr<T> GameObject::AddComponent()
    {
        m_components.push_back(std::make_shared<T>());
        return std::static_pointer_cast<T>(m_components.back());
    }
}  // namespace Sandbox NAMESPACE()

File_GameObject_GENERATED
