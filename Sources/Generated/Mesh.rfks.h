#pragma once

#include "../Engine/EntityComponent/Components/Mesh.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_12403201434061474472() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Mesh>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_12403201434061474472(rfk::generated::getNamespaceFragment_7028080843842788705u_12403201434061474472());
 }
rfk::Class const& Sandbox::Mesh::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Mesh", 17055644869723107221u, sizeof(Mesh), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<IComponent>(), static_cast<rfk::EAccessSpecifier>(1));
Mesh::_rfk_registerChildClass<Mesh>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Mesh>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Mesh>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Mesh>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Mesh>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Mesh>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Mesh>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::Mesh::getArchetype() const noexcept { return Mesh::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Mesh>() noexcept { return &Sandbox::Mesh::staticGetArchetype(); }


