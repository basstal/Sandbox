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


#define Sandbox_ISerializable_GENERATED	\
RFK_UNPACK_IF_NOT_PARSING(friend rfk::internal::CodeGenerationHelpers;\
friend rfk::internal::implements_template1__rfk_registerChildClass<ISerializable<T>, void, void(rfk::Struct&)>; \
\
private: template <typename ChildClass> static void _rfk_registerChildClass(rfk::Struct& childClass) noexcept {\
rfk::Struct const& thisClass = staticGetArchetype();\
if constexpr (!std::is_same_v<ChildClass, ISerializable<T>>)const_cast<rfk::Struct&>(thisClass).addSubclass(childClass, rfk::internal::CodeGenerationHelpers::computeClassPointerOffset<ChildClass, ISerializable<T>>());\
else\
{\
childClass.setFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< rfk::Object>()); childClass.setStaticFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< rfk::Object>()); \
}\
rfk::internal::CodeGenerationHelpers::registerChildClass<rfk::Object, ChildClass>(childClass);\
}\
\
public: static rfk::ClassTemplateInstantiation const& staticGetArchetype() noexcept {\
static bool initialized = false;\
static rfk::ClassTemplateInstantiation type("ISerializable",std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@ISerializable") + rfk::internal::getTypename<Sandbox::ISerializable<T>>()), sizeof(Sandbox::ISerializable<T>), 1, *rfk::getArchetype<::Sandbox::ISerializable>());\
if (!initialized) {\
initialized = true;\
static rfk::TypeTemplateArgument arg0(rfk::getType<T>());\
type.addTemplateArgument(arg0);\
type.setDirectParentsCapacity(1);\
type.addDirectParent(rfk::getArchetype<rfk::Object>(), static_cast<rfk::EAccessSpecifier>(1));\
ISerializable<T>::_rfk_registerChildClass<ISerializable<T>>(type);\
static rfk::StaticMethod defaultSharedInstantiator("", 0u, rfk::getType<rfk::SharedPtr<ISerializable<T>>>(),new rfk::NonMemberFunction<rfk::SharedPtr<ISerializable<T>>()>(&rfk::internal::CodeGenerationHelpers::defaultSharedInstantiator<ISerializable<T>>),rfk::EMethodFlags::Default, nullptr);\
type.addSharedInstantiator(defaultSharedInstantiator);\
static rfk::StaticMethod defaultUniqueInstantiator("", 0u, rfk::getType<rfk::UniquePtr<ISerializable<T>>>(),new rfk::NonMemberFunction<rfk::UniquePtr<ISerializable<T>>()>(&rfk::internal::CodeGenerationHelpers::defaultUniqueInstantiator<ISerializable<T>>),rfk::EMethodFlags::Default, nullptr);\
type.addUniqueInstantiator(defaultUniqueInstantiator);\
[[maybe_unused]] rfk::Method* method = nullptr; [[maybe_unused]] rfk::StaticMethod* staticMethod = nullptr;\
type.setMethodsCapacity(2u); type.setStaticMethodsCapacity(0u); \
method = type.addMethod("SerializeToYaml", std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@ISerializable@F@SerializeToYaml#") + rfk::internal::getTypename<Sandbox::ISerializable<T>>()), rfk::getType<YAML::Node>(), new rfk::MemberFunction<ISerializable<T>, YAML::Node ()>(static_cast<YAML::Node (ISerializable<T>::*)()>(& ISerializable<T>::SerializeToYaml)), static_cast<rfk::EMethodFlags>(1));\
method = type.addMethod("DeserializeFromYaml", std::hash<std::string>()(std::string("c:@N@Sandbox@ST>1#T@ISerializable@F@DeserializeFromYaml#&1$@N@YAML@S@Node#") + rfk::internal::getTypename<Sandbox::ISerializable<T>>()), rfk::getType<bool>(), new rfk::MemberFunction<ISerializable<T>, bool (const YAML::Node &)>(static_cast<bool (ISerializable<T>::*)(const YAML::Node &)>(& ISerializable<T>::DeserializeFromYaml)), static_cast<rfk::EMethodFlags>(1));\
method->setParametersCapacity(1); \
method->addParameter("node", 0u, rfk::getType<const YAML::Node &>());\
\
}\
return type; }\
\
)\


#define File_ISerializable_GENERATED	\
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::ISerializable>() noexcept;\


