#pragma once

#include "../Math/Vector3.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_7616813283074115459() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Vector3>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_7616813283074115459(rfk::generated::getNamespaceFragment_7028080843842788705u_7616813283074115459());
 }
rfk::Class const& Sandbox::Vector3::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Vector3", 8269834297046515124u, sizeof(Vector3), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable<Vector3>>(), static_cast<rfk::EAccessSpecifier>(1));
Vector3::_rfk_registerChildClass<Vector3>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Vector3>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Vector3>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Vector3>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Vector3>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Vector3>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Vector3>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Vector3>() noexcept { return &Sandbox::Vector3::staticGetArchetype(); }


