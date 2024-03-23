#pragma once

#include "../TestRefureku/TestProperty.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_10400732428136878097() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::CameraProperty>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_10400732428136878097(rfk::generated::getNamespaceFragment_7028080843842788705u_10400732428136878097());
 }
rfk::Struct const& Sandbox::CameraProperty::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Struct type("CameraProperty", 18219367254969458526u, sizeof(CameraProperty), 0);
if (!initialized) {
initialized = true;
CameraProperty::_rfk_registerChildClass<CameraProperty>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<CameraProperty>>(),new rfk::NonMemberFunction<rfk::SharedPtr<CameraProperty>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<CameraProperty>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<CameraProperty>>(),new rfk::NonMemberFunction<rfk::UniquePtr<CameraProperty>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<CameraProperty>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::CameraProperty>() noexcept { return &Sandbox::CameraProperty::staticGetArchetype(); }


