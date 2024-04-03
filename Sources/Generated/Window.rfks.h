#pragma once

#include "../Platform/Window.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_7029717731215040302() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::Window>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_7029717731215040302(rfk::generated::getNamespaceFragment_7028080843842788705u_7029717731215040302());
 }
rfk::Class const& Sandbox::Window::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("Window", 16464447535988009276u, sizeof(Window), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(2);
type.addDirectParent(rfk::getArchetype<std::enable_shared_from_this<Window>>(), static_cast<rfk::EAccessSpecifier>(1));
type.addDirectParent(rfk::getArchetype<ISerializable<Window>>(), static_cast<rfk::EAccessSpecifier>(1));
Window::_rfk_registerChildClass<Window>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<Window>>(),new rfk::NonMemberFunction<rfk::SharedPtr<Window>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<Window>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<Window>>(),new rfk::NonMemberFunction<rfk::UniquePtr<Window>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<Window>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
type.setMethodsCapacity(0u); type.setStaticMethodsCapacity(0u); 
}
return type; }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::Window>() noexcept { return &Sandbox::Window::staticGetArchetype(); }


