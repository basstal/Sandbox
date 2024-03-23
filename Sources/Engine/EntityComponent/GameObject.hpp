#pragma once
#include <memory>
#include <string>
#include <vector>


namespace Sandbox
{
    class Transform;
    class IComponent;

    template <typename T>
    concept DerivedFromIComponent = std::is_base_of_v<IComponent, T>;

    class GameObject
    {
    public:
        std::shared_ptr<Transform> transform;
        std::string name;
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

    private:
        bool m_cleaned = false;

        std::vector<std::shared_ptr<IComponent>> m_components;
    };


    template <DerivedFromIComponent T>
    std::shared_ptr<T> GameObject::GetComponent()
    {
        for (auto& component : m_components)
        {
            if (component)
            {
                return std::dynamic_pointer_cast<T>(component);
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
}
