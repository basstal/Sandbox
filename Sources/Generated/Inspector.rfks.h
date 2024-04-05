#pragma once

#include "../Editor/ImGuiWindows/Inspector.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_1075689188831286487() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Inspector>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_1075689188831286487(rfk::generated::getNamespaceFragment_7028080843842788705u_1075689188831286487());
 }
rfk::Class const& Sandbox::Inspector::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Inspector", 3173275120126359657u, sizeof(Inspector), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<IImGuiWindow>(), static_cast<rfk::EAccessSpecifier>(1));
Inspector::_rfk_registerChildClass<Inspector>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Inspector>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Inspector>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Inspector>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Inspector>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Inspector>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Inspector>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Inspector>() noexcept { return &Sandbox::Inspector::staticGetArchetype(); }


