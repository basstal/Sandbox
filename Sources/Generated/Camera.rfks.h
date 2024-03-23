#pragma once

#include "../Engine/Camera.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_15521859494802656014() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Camera>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_15521859494802656014(rfk::generated::getNamespaceFragment_7028080843842788705u_15521859494802656014());
 }
rfk::Class const& Sandbox::Camera::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Camera", 10866052711324808399u, sizeof(Camera), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable<Camera>>(), static_cast<rfk::EAccessSpecifier>(1));
Camera::_rfk_registerChildClass<Camera>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Camera>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Camera>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Camera>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Camera>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Camera>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Camera>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Camera>() noexcept { return &Sandbox::Camera::staticGetArchetype(); }


