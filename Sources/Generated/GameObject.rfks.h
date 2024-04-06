#pragma once

#include "../Engine/EntityComponent/GameObject.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_16378262817912033374() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(2u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::IComponent>());
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::GameObject>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_16378262817912033374(rfk::generated::getNamespaceFragment_7028080843842788705u_16378262817912033374());
 }
rfk::Class const& Sandbox::GameObject::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("GameObject", 7774748035391891929u, sizeof(GameObject), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable>(), static_cast<rfk::EAccessSpecifier>(1));
GameObject::_rfk_registerChildClass<GameObject>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<GameObject>>(),new rfk::NonMemberFunction<rfk::SharedPtr<GameObject>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<GameObject>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<GameObject>>(),new rfk::NonMemberFunction<rfk::UniquePtr<GameObject>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<GameObject>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::GameObject::getArchetype() const noexcept { return GameObject::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::GameObject>() noexcept { return &Sandbox::GameObject::staticGetArchetype(); }


