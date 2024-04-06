#pragma once

#include "../Serialization/ISerializable.hpp"

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
 static rfk::NamespaceFragment const& getNamespaceFragment_7028080843842788705u_14392375358218912163() noexcept {
static rfk::NamespaceFragment fragment("Sandbox", 7028080843842788705u);
static bool initialized = false;
if (!initialized) {
initialized = true;
fragment.setNestedEntitiesCapacity(1u);
fragment.addNestedEntity(*rfk::getArchetype<Sandbox::ISerializable>());
}
return fragment; }
static rfk::NamespaceFragmentRegisterer const namespaceFragmentRegisterer_7028080843842788705u_14392375358218912163(rfk::generated::getNamespaceFragment_7028080843842788705u_14392375358218912163());
 }
rfk::Class const& Sandbox::ISerializable::staticGetArchetype() noexcept {
static bool initialized = false;
static rfk::Class type("ISerializable", 16012415956368750672u, sizeof(ISerializable), 1);
if (!initialized) {
initialized = true;
type.setDirectParentsCapacity(1);
type.addDirectParent(rfk::getArchetype<rfk::Object>(), static_cast<rfk::EAccessSpecifier>(1));
ISerializable::_rfk_registerChildClass<ISerializable>(type);
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<ISerializable>>(),new rfk::NonMemberFunction<rfk::SharedPtr<ISerializable>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<ISerializable>),rfk::EMethodFlags::Default, nullptr);
type.addSharedInstantiator(defaultSharedInstantiator);
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<ISerializable>>(),new rfk::NonMemberFunction<rfk::UniquePtr<ISerializable>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<ISerializable>),rfk::EMethodFlags::Default, nullptr);
type.addUniqueInstantiator(defaultUniqueInstantiator);
[[maybe_unused]] rfk::Method* method = nullptr; [[maybe_unused]] rfk::StaticMethod* staticMethod = nullptr;
type.setMethodsCapacity(2u); type.setStaticMethodsCapacity(0u); 
method = type.addMethod("SerializeToYaml", 15306307837170004511u, rfk::getType<YAML::Node>(), new rfk::MemberFunction<ISerializable, YAML::Node ()>(static_cast<YAML::Node (ISerializable::*)()>(& ISerializable::SerializeToYaml)), static_cast<rfk::EMethodFlags>(1));
method = type.addMethod("DeserializeFromYaml", 4385964492517204887u, rfk::getType<bool>(), new rfk::MemberFunction<ISerializable, bool (const YAML::Node &)>(static_cast<bool (ISerializable::*)(const YAML::Node &)>(& ISerializable::DeserializeFromYaml)), static_cast<rfk::EMethodFlags>(1));
method->setParametersCapacity(1); 
method->addParameter("node", 0u, rfk::getType<const YAML::Node &>());

}
return type; }

rfk::Class const& Sandbox::ISerializable::getArchetype() const noexcept { return ISerializable::staticGetArchetype(); }

template <> rfk::Archetype const* rfk::getArchetype<Sandbox::ISerializable>() noexcept { return &Sandbox::ISerializable::staticGetArchetype(); }


