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


#define Sandbox_Camera_GENERATED	\
RFK_UNPACK_IF_NOT_PARSING(friend rfk::internal::CodeGenerationHelpers;\
friend rfk::internal::implements_template1__rfk_registerChildClass<Camera, void, void(rfk::Struct&)>; \
\
private: template <typename ChildClass> static void _rfk_registerChildClass(rfk::Struct& childClass) noexcept {\
rfk::Struct const& thisClass = staticGetArchetype();\
if constexpr (!std::is_same_v<ChildClass, Camera>)const_cast<rfk::Struct&>(thisClass).addSubclass(childClass, rfk::internal::CodeGenerationHelpers::computeClassPointerOffset<ChildClass, Camera>());\
else\
{\
childClass.setFieldsCapacity(7u + rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< ISerializable<Camera>>()); childClass.setStaticFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< ISerializable<Camera>>()); \
}\
[[maybe_unused]] rfk::Field* field = nullptr; [[maybe_unused]] rfk::StaticField* staticField = nullptr;\
)\
__RFK_DISABLE_WARNING_PUSH \
__RFK_DISABLE_WARNING_OFFSETOF \
RFK_UNPACK_IF_NOT_PARSING(field = childClass.addField("position", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@position") + rfk::internal::getTypename<ChildClass>()), rfk::getType<Vector3>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, position), &thisClass);\
field = childClass.addField("rotationX", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@rotationX") + rfk::internal::getTypename<ChildClass>()), rfk::getType<float>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, rotationX), &thisClass);\
field = childClass.addField("rotationZ", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@rotationZ") + rfk::internal::getTypename<ChildClass>()), rfk::getType<float>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, rotationZ), &thisClass);\
field = childClass.addField("fieldOfView", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@fieldOfView") + rfk::internal::getTypename<ChildClass>()), rfk::getType<float>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, fieldOfView), &thisClass);\
field = childClass.addField("nearPlane", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@nearPlane") + rfk::internal::getTypename<ChildClass>()), rfk::getType<float>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, nearPlane), &thisClass);\
field = childClass.addField("farPlane", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@farPlane") + rfk::internal::getTypename<ChildClass>()), rfk::getType<float>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, farPlane), &thisClass);\
field = childClass.addField("aspectRatio", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Camera@FI@aspectRatio") + rfk::internal::getTypename<ChildClass>()), rfk::getType<float>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, aspectRatio), &thisClass);\
)\
__RFK_DISABLE_WARNING_POP \
RFK_UNPACK_IF_NOT_PARSING(rfk::internal::CodeGenerationHelpers::registerChildClass<ISerializable<Camera>, ChildClass>(childClass);\
}\
\
public:  static rfk::Class const& staticGetArchetype() noexcept;\
\
)\


#define File_Camera_GENERATED	\
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::Camera>() noexcept;\

