#include "pch.hpp"

#include "GameObject.hpp"

#include "Components/Transform.hpp"
#include "Generated/GameObject.rfks.h"
#include "IComponent.hpp"

Sandbox::GameObject::GameObject() { transform = AddComponent<Transform>(); }

Sandbox::GameObject::~GameObject() { Cleanup(); }

void Sandbox::GameObject::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    m_cleaned = true;
    for (auto& component : m_components)
    {
        component->Cleanup();
    }
}

void Sandbox::GameObject::AddComponent(std::shared_ptr<IComponent> component) { m_components.push_back(component); }

std::vector<std::shared_ptr<Sandbox::IComponent>> Sandbox::GameObject::GetComponents() { return m_components; }
