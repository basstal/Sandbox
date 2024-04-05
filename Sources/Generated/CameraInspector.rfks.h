#pragma once

#include "../Editor/ImGuiWindows/ComponentInspectors/CameraInspector.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_16147760089397086743() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::CameraInspector>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_16147760089397086743(rfk::generated::getNamespaceFragment_7028080843842788705u_16147760089397086743());
 }
rfk::Class const& Sandbox::CameraInspector::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("CameraInspector", 14067640434120445984u, sizeof(CameraInspector), 1);
if (!initialized) {
initialized = true;
type.setPropertiesCapacity(1);
static_assert((InspectComponentName::targetEntityKind & rfk::EEntityKind::Class) != rfk::EEntityKind::Undefined, "[Refureku] InspectComponentName can't be applied to a rfk::EEntityKind::Class");static InspectComponentName property_14067640434120445984u_0{"Camera"};type.addProperty(property_14067640434120445984u_0);
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<Inspector>(), static_cast<rfk::EAccessSpecifier>(1));
CameraInspector::_rfk_registerChildClass<CameraInspector>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<CameraInspector>>(),new rfk::NonMemberFunction<rfk::SharedPtr<CameraInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<CameraInspector>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<CameraInspector>>(),new rfk::NonMemberFunction<rfk::UniquePtr<CameraInspector>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<CameraInspector>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::CameraInspector>() noexcept { return &Sandbox::CameraInspector::staticGetArchetype(); }


