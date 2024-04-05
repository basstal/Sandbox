#pragma once

#include "../Editor/ImGuiWindows/ComponentInspectors/MeshInspector.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_15092272503555815797() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::MeshInspector>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_15092272503555815797(rfk::generated::getNamespaceFragment_7028080843842788705u_15092272503555815797());
 }
rfk::Class const& Sandbox::MeshInspector::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("MeshInspector", 11719653579870390718u, sizeof(MeshInspector), 1);
if (!initialized) {
initialized = true;
type.setPropertiesCapacity(1);
static_assert((InspectComponentName::targetEntityKind & rfk::EEntityKind::Class) != rfk::EEntityKind::Undefined, "[Refureku] InspectComponentName can't be applied to a rfk::EEntityKind::Class");static InspectComponentName property_11719653579870390718u_0{"Mesh"};type.addProperty(property_11719653579870390718u_0);
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<Inspector>(), static_cast<rfk::EAccessSpecifier>(1));
MeshInspector::_rfk_registerChildClass<MeshInspector>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<MeshInspector>>(),new rfk::NonMemberFunction<rfk::SharedPtr<MeshInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<MeshInspector>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<MeshInspector>>(),new rfk::NonMemberFunction<rfk::UniquePtr<MeshInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<MeshInspector>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::MeshInspector>() noexcept { return &Sandbox::MeshInspector::staticGetArchetype(); }


