#pragma once

#include "../Editor/ImGuiWindows/Viewport.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_8421790314007007932() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(2u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Camera>());
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Viewport>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_8421790314007007932(rfk::generated::getNamespaceFragment_7028080843842788705u_8421790314007007932());
 }
rfk::Class const& Sandbox::Viewport::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Viewport", 10006828136490184082u, sizeof(Viewport), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<IImGuiWindow>(), static_cast<rfk::EAccessSpecifier>(1));
Viewport::_rfk_registerChildClass<Viewport>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Viewport>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Viewport>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Viewport>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Viewport>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Viewport>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Viewport>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::Viewport::getArchetype() const noexcept { return Viewport::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Viewport>() noexcept { return &Sandbox::Viewport::staticGetArchetype(); }


