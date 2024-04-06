#include "pch.hpp"

#include "IComponent.hpp"

#include "Generated/IComponent.rfks.h"

std::string Sandbox::IComponent::GetDerivedClassName()
{
    return this->getArchetype().getName();
}
void Sandbox::IComponent::Cleanup() {}
