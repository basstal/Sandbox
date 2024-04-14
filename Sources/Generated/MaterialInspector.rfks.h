#pragma once

#include "../Editor/ImGuiWindows/ComponentInspectors/MaterialInspector.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_14755918293945287257() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::MaterialInspector>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_14755918293945287257(rfk::generated::getNamespaceFragment_7028080843842788705u_14755918293945287257());
 }
rfk::Class const& Sandbox::MaterialInspector::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("MaterialInspector", 16635722917542463514u, sizeof(MaterialInspector), 1);
if (!initialized) {
initialized = true;
type.setPropertiesCapacity(1);
static_assert((InspectComponentName::targetEntityKind & rfk::EEntityKind::Class) != rfk::EEntityKind::Undefined, "[Refureku] InspectComponentName can't be applied to a rfk::EEntityKind::Class");static InspectComponentName property_16635722917542463514u_0{"Material"};type.addProperty(property_16635722917542463514u_0);
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<Inspector>(), static_cast<rfk::EAccessSpecifier>(1));
MaterialInspector::_rfk_registerChildClass<MaterialInspector>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<MaterialInspector>>(),new rfk::NonMemberFunction<rfk::SharedPtr<MaterialInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<MaterialInspector>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<MaterialInspector>>(),new rfk::NonMemberFunction<rfk::UniquePtr<MaterialInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<MaterialInspector>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::MaterialInspector::getArchetype() const noexcept { return MaterialInspector::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::MaterialInspector>() noexcept { return &Sandbox::MaterialInspector::staticGetArchetype(); }


