#pragma once

#include "../Misc/String.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_18329063851905182782() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::String>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_18329063851905182782(rfk::generated::getNamespaceFragment_7028080843842788705u_18329063851905182782());
 }
rfk::Class const& Sandbox::String::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("String", 4726623101810144873u, sizeof(String), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable>(), static_cast<rfk::EAccessSpecifier>(1));
String::_rfk_registerChildClass<String>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<String>>(),new rfk::NonMemberFunction<rfk::SharedPtr<String>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<String>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<String>>(),new rfk::NonMemberFunction<rfk::UniquePtr<String>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<String>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
[[maybe_unused]] rfk::Method* method = nullptr; [[maybe_unused]] rfk::StaticMethod* staticMethod = nullptr;
type.setMethodsCapacity(1u); type.setStaticMethodsCapacity(0u); 
method = type.addMethod("Construct", 4695711749284567644u, rfk::getType<void>(), new rfk::MemberFunction<String, void (const std::string &)>(static_cast<void (String::*)(const std::string &)>(& String::Construct)), static_cast<rfk::EMethodFlags>(1));
method->setParametersCapacity(1); 
method->addParameter("inString", 0u, rfk::getType<const std::string &>());

}
return type; }

rfk::Class const& Sandbox::String::getArchetype() const noexcept { return String::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::String>() noexcept { return &Sandbox::String::staticGetArchetype(); }


