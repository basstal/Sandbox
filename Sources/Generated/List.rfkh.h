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


#define Sandbox_List_GENERATED	\
RFK_UNPACK_IF_NOT_PARSING(friend rfk::internal::CodeGenerationHelpers;\
friend rfk::internal::implements_template1__rfk_registerChildClass<List<T>, void, void(rfk::Struct&)>; \
\
private: template <typename ChildClass> static void _rfk_registerChildClass(rfk::Struct& childClass) noexcept {\
rfk::Struct const& thisClass = staticGetArchetype();\
if constexpr (!std::is_same_v<ChildClass, List<T>>)const_cast<rfk::Struct&>(thisClass).addSubclass(childClass, rfk::internal::CodeGenerationHelpers::computeClassPointerOffset<ChildClass, List<T>>());\
else\
{\
childClass.setFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< std::vector<T>>()+rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< rfk::Object>()); childClass.setStaticFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< std::vector<T>>()+rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< rfk::Object>()); \
}\
rfk::internal::CodeGenerationHelpers::registerChildClass<std::vector<T>, ChildClass>(childClass);\
rfk::internal::CodeGenerationHelpers::registerChildClass<rfk::Object, ChildClass>(childClass);\
}\
\
public: static rfk::ClassTemplateInstantiation const& staticGetArchetype() noexcept {\
static bool initialized = false;\
static rfk::ClassTemplateInstantiation type("List",std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@List") + rfk::internal::getTypename<Sandbox::List<T>>()), sizeof(Sandbox::List<T>), 1, *rfk::getArchetype<::Sandbox::List>());\
if (!initialized) {\
initialized = true;\
static rfk::TypeTemplateArgument arg0(rfk::getType<T>());\
type.addTemplateArgument(arg0);\
type.setDirectParentsCapacity(2);\
type.addDirectParent(rfk::getArchetype<std::vector<T>>(), static_cast<rfk::EAccessSpecifier>(1));\
type.addDirectParent(rfk::getArchetype<rfk::Object>(), static_cast<rfk::EAccessSpecifier>(1));\
List<T>::_rfk_registerChildClass<List<T>>(type);\
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<List<T>>>(),new rfk::NonMemberFunction<rfk::SharedPtr<List<T>>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<List<T>>),rfk::EMethodFlags::Default, nullptr);\
type.addSharedInstantiator(defaultSharedInstantiator);\
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<List<T>>>(),new rfk::NonMemberFunction<rfk::UniquePtr<List<T>>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<List<T>>),rfk::EMethodFlags::Default, nullptr);\
type.addUniqueInstantiator(defaultUniqueInstantiator);\
[[maybe_unused]] rfk::Method* method = nullptr; [[maybe_unused]] rfk::StaticMethod* staticMethod = nullptr;\
type.setMethodsCapacity(2u); type.setStaticMethodsCapacity(0u); \
method = type.addMethod("SerializeToYaml", std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@List@F@SerializeToYaml#") + rfk::internal::getTypename<Sandbox::List<T>>()), rfk::getType<YAML::Node>(), new rfk::MemberFunction<List<T>, YAML::Node ()>(static_cast<YAML::Node (List<T>::*)()>(& List<T>::SerializeToYaml)), static_cast<rfk::EMethodFlags>(1));\
method = type.addMethod("DeserializeFromYaml", std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@List@F@DeserializeFromYaml#&1$@N@YAML@S@Node#") + rfk::internal::getTypename<Sandbox::List<T>>()), rfk::getType<bool>(), new rfk::MemberFunction<List<T>, bool (const YAML::Node &)>(static_cast<bool (List<T>::*)(const YAML::Node &)>(& List<T>::DeserializeFromYaml)), static_cast<rfk::EMethodFlags>(1));\
method->setParametersCapacity(1); \
method->addParameter("node", 0u, rfk::getType<const YAML::Node &>());\
\
}\
return type; }\
\
)\


#define File_List_GENERATED	\
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::List>() noexcept;\


