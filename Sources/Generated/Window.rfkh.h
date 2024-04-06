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


#define Sandbox_Window_GENERATED	\
RFK_UNPACK_IF_NOT_PARSING(friend rfk::internal::CodeGenerationHelpers;\
friend rfk::internal::implements_template1__rfk_registerChildClass<Window, void, void(rfk::Struct&)>; \
\
private: template <typename ChildClass> static void _rfk_registerChildClass(rfk::Struct& childClass) noexcept {\
rfk::Struct const& thisClass = staticGetArchetype();\
if constexpr (!std::is_same_v<ChildClass, Window>)const_cast<rfk::Struct&>(thisClass).addSubclass(childClass, rfk::internal::CodeGenerationHelpers::computeClassPointerOffset<ChildClass, Window>());\
else\
{\
childClass.setFieldsCapacity(5u + rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< std::enable_shared_from_this<Window>>()+rfk::internal::CodeGenerationHelpers::getReflectedFieldsCount< ISerializable>()); childClass.setStaticFieldsCapacity(0u + rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< std::enable_shared_from_this<Window>>()+rfk::internal::CodeGenerationHelpers::getReflectedStaticFieldsCount< ISerializable>()); \
}\
[[maybe_unused]] rfk::Field* field = nullptr; [[maybe_unused]] rfk::StaticField* staticField = nullptr;\
)\
__RFK_DISABLE_WARNING_PUSH \
__RFK_DISABLE_WARNING_OFFSETOF \
RFK_UNPACK_IF_NOT_PARSING(field = childClass.addField("isWindow", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Window@FI@isWindow") + rfk::internal::getTypename<ChildClass>()), rfk::getType<bool>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, isWindow), &thisClass);\
field = childClass.addField("positionX", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Window@FI@positionX") + rfk::internal::getTypename<ChildClass>()), rfk::getType<int>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, positionX), &thisClass);\
field = childClass.addField("positionY", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Window@FI@positionY") + rfk::internal::getTypename<ChildClass>()), rfk::getType<int>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, positionY), &thisClass);\
field = childClass.addField("width", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Window@FI@width") + rfk::internal::getTypename<ChildClass>()), rfk::getType<int>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, width), &thisClass);\
field = childClass.addField("height", std::hash<std::string>()(std::string("c:@N@Sandbox@S@Window@FI@height") + rfk::internal::getTypename<ChildClass>()), rfk::getType<int>(), static_cast<rfk::EFieldFlags>(1), offsetof(ChildClass, height), &thisClass);\
)\
__RFK_DISABLE_WARNING_POP \
RFK_UNPACK_IF_NOT_PARSING(rfk::internal::CodeGenerationHelpers::registerChildClass<std::enable_shared_from_this<Window>, ChildClass>(childClass);\
rfk::internal::CodeGenerationHelpers::registerChildClass<ISerializable, ChildClass>(childClass);\
}\
\
public:  static rfk::Class const& staticGetArchetype() noexcept;\
\
public:  virtual rfk::Class const& getArchetype() const noexcept override;\
\
)\


#define File_Window_GENERATED	\
template <>  rfk::Archetype const* rfk::getArchetype<Sandbox::Window>() noexcept;\


