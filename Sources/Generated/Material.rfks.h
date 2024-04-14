#pragma once

#include "../Engine/EntityComponent/Components/Material.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_4438328431205303940() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Material>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_4438328431205303940(rfk::generated::getNamespaceFragment_7028080843842788705u_4438328431205303940());
 }
rfk::Class const& Sandbox::Material::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Material", 12109211038171909497u, sizeof(Material), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<IComponent>(), static_cast<rfk::EAccessSpecifier>(1));
Material::_rfk_registerChildClass<Material>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Material>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Material>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Material>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Material>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Material>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Material>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::Material::getArchetype() const noexcept { return Material::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Material>() noexcept { return &Sandbox::Material::staticGetArchetype(); }


