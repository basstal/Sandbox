#pragma once

#include "../Engine/EntityComponent/IComponent.hpp"

#include <type_traits>
#include <Refureku/TypeInfo/Entity/DefaultEntityRegisterer.h>
#include <Refureku/TypeInfo/Archetypes/ArchetypeRegisterer.h>
#include <Refureku/TypeInfo/Namespace/Namespace.h>
#include <Refureku/TypeInfo/Namespace/NamespaceFragment.h>
#include <Refureku/TypeInfo/Namespace/NamespaceFragmentRegisterer.h>
#include <Refureku/TypeInfo/Archetypes/Template/TypeTemplateParameter.h>
#include <Refureku/TypeInfo/Archetypes/Template/NonTypeTemplateParameter.h>
#include <Refureku/TypeInfo/Archetypes/Template/TemplateTemplateParameter.h>

namespace rfk::generated { 
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_3739433147908472249() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::IComponent>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_3739433147908472249(rfk::generated::getNamespaceFragment_7028080843842788705u_3739433147908472249());
 }
rfk::Class const& Sandbox::IComponent::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("IComponent", 2358108181402035206u, sizeof(IComponent), 1);
if (!initialized) {
initialized = true;
IComponent::_rfk_registerChildClass<IComponent>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<IComponent>>(),new rfk::NonMemberFunction<rfk::SharedPtr<IComponent>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<IComponent>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<IComponent>>(),new rfk::NonMemberFunction<rfk::UniquePtr<IComponent>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<IComponent>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::IComponent>() noexcept { return &Sandbox::IComponent::staticGetArchetype(); }


