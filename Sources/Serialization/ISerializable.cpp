#include "Generated/ISerializable.rfks.h"
// #include "FileSystem/Logger.hpp"
// #include "glm/vec3.hpp"
// #include "Refureku/Object.h"
// #include "Refureku/TypeInfo/Archetypes/Struct.h"
// #include "Refureku/TypeInfo/Entity/EntityCast.h"
// #include "Refureku/TypeInfo/Variables/Field.h"

// YAML::Node Sandbox::ISerializable::Save(const rttr::instance& object)
// {
// rttr::type objectType = object.get_type();
// YAML::Node node;
//
// // 处理基本类型和枚举类型
// if (objectType.is_arithmetic() || objectType.is_enumeration())
// {
//     LOGF("should not call here")
//     // return YAML::Node(object); // 基本类型和枚举转换为字符串
// }
// if (objectType.is_class())
// {
//     for (auto& prop : objectType.get_properties())
//     {
//         rttr::variant propertyValue = prop.get_value(object);
//         if (!propertyValue)
//             continue; // 无法获取属性值，跳过
//
//         rttr::instance propertyInstance = propertyValue.get_type().get_raw_type().is_wrapper() ? propertyValue.extract_wrapped_value() : propertyValue;
//         node[prop.get_name().to_string()] = Save(propertyInstance); // 递归序列化
//     }
// }
// // 此处可以添加对其他类型的支持，例如容器等
// return node;
// }
//
//
// bool Sandbox::ISerializable::Load(const YAML::Node& node, rttr::instance& object)
// {
//     // rttr::type objectType = object.get_type();
//     // if (objectType.is_arithmetic() || objectType.is_enumeration())
//     // {
//     //     LOGF("should not call here")
//     //     // 对于基本类型和枚举，直接从字符串转换
//     //     // objectType.get_variant().convert(node.as<std::string>()); // 假设可以这样直接转换，实际上可能需要更复杂的逻辑
//     // }
//     // else if (objectType.is_class())
//     // {
//     //     for (auto& prop : objectType.get_properties())
//     //     {
//     //         if (node[prop.get_name().to_string()])
//     //         {
//     //             rttr::variant propertyVariant = prop.get_value(object);
//     //             Load(node[prop.get_name().to_string()], propertyVariant); // 递归反序列化
//     //             if (!prop.set_value(object, propertyVariant))
//     //             {
//     //                 LOGF("set value {} to {} failed", propertyVariant, object)
//     //             }
//     //         }
//     //     }
//     // }
//     // // 处理其他类型...
//     // return true;
//     return true;
// }
//
// bool Sandbox::ISerializable::Load(const YAML::Node& node, rttr::variant& variant)
// {
// }
// struct UserData
// {
//     YAML::Node nodePtr;
//     Sandbox::ISerializable* instancePtr;
// };
//
// YAML::Node Sandbox::ISerializable::Save()
// {
//     // 假设object是一个通过Refureku反射得到的实例
//     // 对象的类型使用Refureku的rkfq::TypeInfo获取
//     const rfk::Class& objectType = getArchetype();
//
//     YAML::Node node;
//     UserData userData{node, this};
//     objectType.foreachField([](rfk::Field const& field, void* inUserData)
//     {
//         const rfk::Type& fieldType = field.getType();
//
//         auto userData = reinterpret_cast<UserData*>(inUserData);
//         // TODO:映射表
//         if (fieldType == rfk::getType<int>())
//         {
//             userData->nodePtr[field.getName()] = field.get<int>(*userData->instancePtr); // 假设Save已经支持处理Refureku的Variant
//         }
//         else if (fieldType == rfk::getType<float>())
//         {
//             userData->nodePtr[field.getName()] = field.get<float>(*userData->instancePtr); // 假设Save已经支持处理Refureku的Variant
//         }
//         else if (fieldType == rfk::getType<glm::vec3>())
//         {
//             userData->nodePtr[field.getName()] = field.get<float>(*userData->instancePtr); // 假设Save已经支持处理Refureku的Variant
//         }
//         else
//         {
//             LOGF("not support type {}", fieldType.getArchetype()->getName())
//         }
//         // // 使用getPropertyValue获取属性值，这里需要一个实例指针
//         // rfk::Variant propertyValue = field.get(*this);
//         //
//         // if (!propertyValue.isValid())
//         //     return; // 无法获取属性值，跳过
//         //
//         // // 对于Refureku，不需要手动处理包装类型
//         // // 直接递归序列化属性值
//         // node[field.getName()] = Save(propertyValue); // 假设Save已经支持处理Refureku的Variant
//         // // 处理基本类型和枚举类型
//         // if (field.getKind() || objectType->isEnum())
//         // {
//         //     LOGF("should not call here")
//         //     // 对于基本类型和枚举，直接转换为YAML节点
//         //     // 注意：你可能需要根据实际情况调整转换逻辑
//         // }
//         // else if (objectType->getKind() == rkfq::ETypeKind::Class || objectType->getKind() == rkfq::ETypeKind::Struct)
//         // {
//         //     // 遍历所有属性
//         //     for (auto const& property : objectType->getProperties())
//         //     {
//         //         // 使用getPropertyValue获取属性值，这里需要一个实例指针
//         //         rkfq::Variant propertyValue = property->get(object);
//         //
//         //         if (!propertyValue.isValid())
//         //             continue; // 无法获取属性值，跳过
//         //
//         //         // 对于Refureku，不需要手动处理包装类型
//         //         // 直接递归序列化属性值
//         //         node[property->getName()] = Save(propertyValue); // 假设Save已经支持处理Refureku的Variant
//         //     }
//         // }
//         // 此处可以添加对其他类型的支持，例如容器等
//         return true;
//     }, &userData);
//
//
//     return node;
// }


