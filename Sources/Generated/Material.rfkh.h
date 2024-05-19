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


#define Sandbox_Material_GENERATED	\
RFK_UNPACK_IF_NOT_PARSING(friend rfk::internal::CodeGenerationHelpers;\
friend rfk::internal::implements_template1__rfk_registerChildClass<Material, void, void(rfk::Struct&)>; \
\
private: template <typename ChildClass> static void _rfk_registerChildClass(rfk::Struct& childClass) noexcept {\
rfk::Struct const& thisClass = staticGetArchetype();\
if constexpr (!std::is_same_v<ChildClass, Material>)const_cast<rfk::Struct&>(thisClass).addSubclass(childClass, rfk::internal::CodeGenerationHelpers::computeClassPointerOffset<ChildClass, Material>());\
else\
{\
childClass.setFieldsCapacity(4u + rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< IComponent>()); childClass.setStaticFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< IComponent>()); \
}\
[[maybe_unused]] rfk::Field* field = nullptr; [[maybe_unused]] rfk::StaticField* staticField = nullptr;\
)\
__RFK_DISABLE_WARNING_PUSH \
__RFK_DISABLE_WARNING_OFFSETOF \
RFK_UNPACK_IF_NOT_PARSING(field = childClass.addField("albedoPath", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Material@FI@albedoPath") + rfk::internal::getTypename<ChildClass>()), rfk::getType<String>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, albedoPath), &thisClass);\
field = childClass.addField("metallicPath", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Material@FI@metallicPath") + rfk::internal::getTypename<ChildClass>()), rfk::getType<String>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, metallicPath), &thisClass);\
field = childClass.addField("roughnessPath", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Material@FI@roughnessPath") + rfk::internal::getTypename<ChildClass>()), rfk::getType<String>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, roughnessPath), &thisClass);\
field = childClass.addField("aoPath", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Material@FI@aoPath") + rfk::internal::getTypename<ChildClass>()), rfk::getType<String>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, aoPath), &thisClass);\
)\
__RFK_DISABLE_WARNING_POP \
RFK_UNPACK_IF_NOT_PARSING(rfk::internal::CodeGenerationHelpers::registerChildClass<IComponent, ChildClass>(childClass);\
}\
\
public:  static rfk::Class const& staticGetArchetype() noexcept;\
\
public:  virtual rfk::Class const& getArchetype() const noexcept override;\
\
)\


#define File_Material_GENERATED	\
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::Material>() noexcept;\


