#pragma once

#include "../Editor/ImGuiWindows/ComponentInspectors/TransformInspector.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_11047394569009485868() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::TransformInspector>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_11047394569009485868(rfk::generated::getNamespaceFragment_7028080843842788705u_11047394569009485868());
 }
rfk::Class const& Sandbox::TransformInspector::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("TransformInspector", 8030797055842132733u, sizeof(TransformInspector), 1);
if (!initialized) {
initialized = true;
type.setPropertiesCapacity(1);
static_assert((InspectComponentName::targetEntityKind & rfk::EEntityKind::Class) != rfk::EEntityKind::Undefined, "[Refureku] InspectComponentName can't be applied to a rfk::EEntityKind::Class");static InspectComponentName property_8030797055842132733u_0{"Transform"};type.addProperty(property_8030797055842132733u_0);
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<Inspector>(), static_cast<rfk::EAccessSpecifier>(1));
TransformInspector::_rfk_registerChildClass<TransformInspector>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<TransformInspector>>(),new rfk::NonMemberFunction<rfk::SharedPtr<TransformInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<TransformInspector>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<TransformInspector>>(),new rfk::NonMemberFunction<rfk::UniquePtr<TransformInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<TransformInspector>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::TransformInspector::getArchetype() const noexcept { return TransformInspector::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::TransformInspector>() noexcept { return &Sandbox::TransformInspector::staticGetArchetype(); }


