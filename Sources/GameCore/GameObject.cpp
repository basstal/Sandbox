#include "GameObject.hpp"

#include "IComponent.hpp"
#include "Transform.hpp"

GameObject::GameObject()
{
    transform = std::make_shared<Transform>();
}

GameObject::~GameObject()
{
    Cleanup();
}

void GameObject::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    for (auto& component : components)
    {
        component->Cleanup();
    }
    m_cleaned = true;
}

void GameObject::AddComponent(std::shared_ptr<IComponent> component)
{
    components.push_back(component);
}
