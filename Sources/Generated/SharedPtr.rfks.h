#pragma once

#include "../Serialization/SharedPtr.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_8911568896756117478() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::SharedPtr>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_8911568896756117478(rfk::generated::getNamespaceFragment_7028080843842788705u_8911568896756117478());
 }
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::SharedPtr>() noexcept {
static bool initialized = false;
static rfk::ClassTemplate type("SharedPtr", 3947841890924902311u, 1);
if (!initialized) {
initialized = true;
{ 
static rfk::TypeTemplateParameter templateParameter("T");
type.addTemplateParameter(templateParameter);
}
}return &type; }


