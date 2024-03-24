#pragma once

#include "../Editor/IImGuiWindow.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_15120246445022154840() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::IImGuiWindow>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_15120246445022154840(rfk::generated::getNamespaceFragment_7028080843842788705u_15120246445022154840());
 }
rfk::Class const& Sandbox::IImGuiWindow::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("IImGuiWindow", 18202984137645972520u, sizeof(IImGuiWindow), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<ISerializable<IImGuiWindow>>(), static_cast<rfk::EAccessSpecifier>(1));
IImGuiWindow::_rfk_registerChildClass<IImGuiWindow>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<IImGuiWindow>>(),new rfk::NonMemberFunction<rfk::SharedPtr<IImGuiWindow>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<IImGuiWindow>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<IImGuiWindow>>(),new rfk::NonMemberFunction<rfk::UniquePtr<IImGuiWindow>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<IImGuiWindow>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::IImGuiWindow>() noexcept { return &Sandbox::IImGuiWindow::staticGetArchetype(); }


