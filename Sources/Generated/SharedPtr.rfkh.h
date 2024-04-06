#pragma once

#include "EntityMacros.h"

#include <Refureku/TypeInfo/Entity/EEntityKind.h>
#include <string>
#include <Refureku/Misc/CodeGenerationHelpers.h>
#include <Refureku/Misc/DisableWarningMacros.h>
#include <Refureku/TypeInfo/Functions/Method.h>
#include <Refureku/TypeInfo/Functions/StaticMethod.h>
#include <Refureku/TypeInfo/Variables/Field.h>
#include <Refureku/TypeInfo/Variables/StaticField.h>
#include <Refureku/TypeInfo/Archetypes/Enum.h>
#include <Refureku/TypeInfo/Archetypes/EnumValue.h>
#include <Refureku/TypeInfo/Variables/Variable.h>
#include <Refureku/TypeInfo/Functions/Function.h>
#include <Refureku/TypeInfo/Archetypes/Template/ClassTemplate.h>
#include <Refureku/TypeInfo/Archetypes/Template/ClassTemplateInstantiation.h>
#include <Refureku/TypeInfo/Archetypes/Template/ClassTemplateInstantiationRegisterer.h>
#include <Refureku/TypeInfo/Archetypes/Template/TypeTemplateArgument.h>
#include <Refureku/TypeInfo/Archetypes/Template/NonTypeTemplateArgument.h>
#include <Refureku/TypeInfo/Archetypes/Template/TemplateTemplateArgument.h>


#define Sandbox_SharedPtr_GENERATED	\
RFK_UNPACK_IF_NOT_PARSING(friend rfk::internal::CodeGenerationHelpers;\
friend rfk::internal::implements_template1__rfk_registerChildClass<SharedPtr<T>, void, void(rfk::Struct&)>; \
\
private: template <typename ChildClass> static void _rfk_registerChildClass(rfk::Struct& childClass) noexcept {\
rfk::Struct const& thisClass = staticGetArchetype();\
if constexpr (!std::is_same_v<ChildClass, SharedPtr<T>>)const_cast<rfk::Struct&>(thisClass).addSubclass(childClass, rfk::internal::CodeGenerationHelpers::computeClassPointerOffset<ChildClass, SharedPtr<T>>());\
else\
{\
childClass.setFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< std::shared_ptr<T>>()+rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< rfk::Object>()); childClass.setStaticFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< std::shared_ptr<T>>()+rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< rfk::Object>()); \
}\
rfk::internal::CodeGenerationHelpers::registerChildClass<std::shared_ptr<T>, ChildClass>(childClass);\
rfk::internal::CodeGenerationHelpers::registerChildClass<rfk::Object, ChildClass>(childClass);\
}\
\
public: static rfk::ClassTemplateInstantiation const& staticGetArchetype() noexcept {\
static bool initialized = false;\
static rfk::ClassTemplateInstantiation type("SharedPtr",std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@SharedPtr") + rfk::internal::getTypename<Sandbox::SharedPtr<T>>()), sizeof(Sandbox::SharedPtr<T>), 1, *rfk::getArchetype<::Sandbox::SharedPtr>());\
if (!initialized) {\
initialized = true;\
static rfk::TypeTemplateArgument arg0(rfk::getType<T>());\
type.addTemplateArgument(arg0);\
type.setDirectParentsCapacity(2);\
type.addDirectParent(rfk::getArchetype<std::shared_ptr<T>>(), static_cast<rfk::EAccessSpecifier>(1));\
type.addDirectParent(rfk::getArchetype<rfk::Object>(), static_cast<rfk::EAccessSpecifier>(1));\
SharedPtr<T>::_rfk_registerChildClass<SharedPtr<T>>(type);\
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<SharedPtr<T>>>(),new rfk::NonMemberFunction<rfk::SharedPtr<SharedPtr<T>>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<SharedPtr<T>>),rfk::EMethodFlags::Default, nullptr);\
type.addSharedInstantiator(defaultSharedInstantiator);\
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<SharedPtr<T>>>(),new rfk::NonMemberFunction<rfk::UniquePtr<SharedPtr<T>>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<SharedPtr<T>>),rfk::EMethodFlags::Default, nullptr);\
type.addUniqueInstantiator(defaultUniqueInstantiator);\
[[maybe_unused]] rfk::Method* method = nullptr; [[maybe_unused]] rfk::StaticMethod* staticMethod = nullptr;\
type.setMethodsCapacity(2u); type.setStaticMethodsCapacity(0u); \
method = type.addMethod("SerializeToYaml", std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@SharedPtr@F@SerializeToYaml#") + rfk::internal::getTypename<Sandbox::SharedPtr<T>>()), rfk::getType<YAML::Node>(), new rfk::MemberFunction<SharedPtr<T>, YAML::Node ()>(static_cast<YAML::Node (SharedPtr<T>::*)()>(& SharedPtr<T>::SerializeToYaml)), static_cast<rfk::EMethodFlags>(1));\
method = type.addMethod("DeserializeFromYaml", std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@SharedPtr@F@DeserializeFromYaml#&1$@N@YAML@S@Node#") + rfk::internal::getTypename<Sandbox::SharedPtr<T>>()), rfk::getType<bool>(), new rfk::MemberFunction<SharedPtr<T>, bool (const YAML::Node &)>(static_cast<bool (SharedPtr<T>::*)(const YAML::Node &)>(& SharedPtr<T>::DeserializeFromYaml)), static_cast<rfk::EMethodFlags>(1));\
method->setParametersCapacity(1); \
method->addParameter("node", 0u, rfk::getType<const YAML::Node &>());\
\
}\
return type; }\
\
virtual rfk::ClassTemplateInstantiation const& getArchetype() const noexcept override { return staticGetArchetype(); }\
\
)\


#define File_SharedPtr_GENERATED	\
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::SharedPtr>() noexcept;\


