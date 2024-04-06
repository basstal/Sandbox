#pragma once

#include "../Engine/EntityComponent/Scene.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_473021980657952449() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Scene>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_473021980657952449(rfk::generated::getNamespaceFragment_7028080843842788705u_473021980657952449());
 }
rfk::Class const& Sandbox::Scene::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Scene", 5035160349312317672u, sizeof(Scene), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable>(), static_cast<rfk::EAccessSpecifier>(1));
Scene::_rfk_registerChildClass<Scene>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Scene>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Scene>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Scene>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Scene>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Scene>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Scene>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::Scene::getArchetype() const noexcept { return Scene::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Scene>() noexcept { return &Sandbox::Scene::staticGetArchetype(); }


