#pragma once

#include "../Math/Quaternion.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_4556443701776983857() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Quaternion>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_4556443701776983857(rfk::generated::getNamespaceFragment_7028080843842788705u_4556443701776983857());
 }
rfk::Class const& Sandbox::Quaternion::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Quaternion", 3624716398323429864u, sizeof(Quaternion), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable<Quaternion>>(), static_cast<rfk::EAccessSpecifier>(1));
Quaternion::_rfk_registerChildClass<Quaternion>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Quaternion>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Quaternion>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Quaternion>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Quaternion>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Quaternion>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Quaternion>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Quaternion>() noexcept { return &Sandbox::Quaternion::staticGetArchetype(); }


