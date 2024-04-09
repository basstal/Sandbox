#include "pch.hpp"

#include "IComponent.hpp"

#include "Generated/IComponent.rfks.h"

Sandbox::Event<const std::shared_ptr<Sandbox::IComponent>&> Sandbox::IComponent::onComponentCreate = Sandbox::Event<const std::shared_ptr<Sandbox::IComponent>&>();

// TODO:dont use this, will be remove
std::string Sandbox::IComponent::GetDerivedClassName() { return this->getArchetype().getName(); }

const rfk::Class* Sandbox::IComponent::GetDerivedClass()
{
    LOGW("Engine", "GetDerivedClass not implemented! Serialization this component will fail!")
    return nullptr;
}
void Sandbox::IComponent::Cleanup() {}
