#pragma once

#include "../Engine/EntityComponent/Components/Transform.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_2589005516600741883() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Transform>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_2589005516600741883(rfk::generated::getNamespaceFragment_7028080843842788705u_2589005516600741883());
 }
rfk::Class const& Sandbox::Transform::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Transform", 3612830085661866220u, sizeof(Transform), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(2);
type.addDirectParent(rfk::getArchetype<IComponent>(), static_cast<rfk::EAccessSpecifier>(1));
type.addDirectParent(rfk::getArchetype<ISerializable<Transform>>(), static_cast<rfk::EAccessSpecifier>(1));
Transform::_rfk_registerChildClass<Transform>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Transform>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Transform>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Transform>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Transform>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Transform>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Transform>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Transform>() noexcept { return &Sandbox::Transform::staticGetArchetype(); }


