#include "GameObject.hpp"

#include "IComponent.hpp"
#include "Components/Transform.hpp"

Sandbox::GameObject::GameObject()
{
    transform = AddComponent<Transform>();
}

Sandbox::GameObject::~GameObject()
{
    Cleanup();
}

void Sandbox::GameObject::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    for (auto& component : m_components)
    {
        component->Cleanup();
    }
    m_cleaned = true;
}

void Sandbox::GameObject::AddComponent(std::shared_ptr<IComponent> component)
{
    m_components.push_back(component);
}
