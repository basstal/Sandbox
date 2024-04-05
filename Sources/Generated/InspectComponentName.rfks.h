#pragma once

#include "../Serialization/Property/InspectComponentName.hpp"

#include <type_traits>
#include <Refureku/TypeInfo/Entity/DefaultEntityRegisterer.h>
#include <Refureku/TypeInfo/Archetypes/ArchetypeRegisterer.h>
#include <Refureku/TypeInfo/Namespace/Namespace.h>
#include <Refureku/TypeInfo/Namespace/NamespaceFragment.h>
#include <Refureku/TypeInfo/Namespace/NamespaceFragmentRegisterer.h>
#include <Refureku/TypeInfo/Archetypes/Template/TypeTemplateParameter.h>
#include <Refureku/TypeInfo/Archetypes/Template/NonTypeTemplateParameter.h>
#include <Refureku/TypeInfo/Archetypes/Template/TemplateTemplateParameter.h>

rfk::EEntityKind Sandbox::InspectComponentName::getTargetEntityKind() const noexcept { return targetEntityKind; }
static_assert(std::is_base_of_v<rfk::Property, Sandbox::InspectComponentName>, "[Refureku] Can't attach rfk::PropertySettings property to Sandbox::InspectComponentName as it doesn't inherit from rfk::Property.");
namespace rfk::generated { 
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_12387573627580283348() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::InspectComponentName>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_12387573627580283348(rfk::generated::getNamespaceFragment_7028080843842788705u_12387573627580283348());
 }
rfk::Class const& Sandbox::InspectComponentName::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("InspectComponentName", 858203472977910104u, sizeof(InspectComponentName), 1);
if (!initialized) {
initialized = true;
type.setPropertiesCapacity(1);
static_assert((rfk::PropertySettings::targetEntityKind & rfk::EEntityKind::Class) != rfk::EEntityKind::Undefined, "[Refureku] rfk::PropertySettings can't be applied to a rfk::EEntityKind::Class");static rfk::PropertySettings property_858203472977910104u_0{rfk::EEntityKind::Struct | rfk::EEntityKind::Class};type.addProperty(property_858203472977910104u_0);
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<rfk::Property>(), static_cast<rfk::EAccessSpecifier>(1));
InspectComponentName::_rfk_registerChildClass<InspectComponentName>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<InspectComponentName>>(),new rfk::NonMemberFunction<rfk::SharedPtr<InspectComponentName>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<InspectComponentName>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<InspectComponentName>>(),new rfk::NonMemberFunction<rfk::UniquePtr<InspectComponentName>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<InspectComponentName>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

rfk::Class const& Sandbox::InspectComponentName::getArchetype() const noexcept { return InspectComponentName::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::InspectComponentName>() noexcept { return &Sandbox::InspectComponentName::staticGetArchetype(); }


