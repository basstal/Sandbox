#include "pch.hpp"

#include "ISerializable.hpp"

#include <Refureku/TypeInfo/Archetypes/Struct.h>
#include <Refureku/TypeInfo/Entity/EntityCast.h>
#include <Refureku/TypeInfo/Functions/Method.h>
#include <Refureku/TypeInfo/Type.h>
#include <Refureku/TypeInfo/Variables/Field.h>
#include <Refureku/TypeInfo/Variables/Variable.h>

#include "Generated/ISerializable.rfks.h"
#include "Misc/Debug.hpp"

// template <typename T>
void Sandbox::ISerializable::LoadFromFile(const File& file)
{
    if (!file.Exists())
    {
        LOGW_OLD("Cant LoadFromFile : file {} not exists", file.path.string())
        return;
    }
    auto node    = YAML::LoadFile(file.path.string());
    auto success = DeserializeFromYaml(node);
    assert(success && "DeserializeFromYaml failed");
}

// template <typename T>
void Sandbox::ISerializable::SaveToFile(const File& file)
{
    if (!file.Exists())
    {
        file.CreateDirectory();
    }
    auto          node = SerializeToYaml();
    std::ofstream fout(file.path);
    fout << node;
}

struct OutUserData
{
    const YAML::Node* node;
    void*             instancePtr;
};

struct InUserData
{
    YAML::Node node;
    void*      instancePtr;
};

inline bool SerializeObjectField(const rfk::Field& field, void* inUserData)
{
    const rfk::Type& fieldType = field.getType();
    auto             archetype = fieldType.getArchetype();
    if (archetype == nullptr)
    {
        LOGW_OLD("archeType is nullptr. skip this field '{}', isCArray {}, isPointer {}", field.getName(), fieldType.isCArray(), fieldType.isPointer())
        return true;
    }
    const rfk::Class* classArcheType = rfk::classCast(archetype);
    LOGD_OLD("field : {} , fieldType : {}, isPointer : {}, isValue : {}", field.getName(), archetype->getName(), fieldType.isPointer(), fieldType.isValue())
    auto userData = static_cast<InUserData*>(inUserData);
    // TODO:映射表
    if (fieldType.match(rfk::getType<int>()))
    {
        userData->node[field.getName()] = field.getUnsafe<int>(userData->instancePtr);
    }
    else if (fieldType.match(rfk::getType<float>()))
    {
        // auto owner = field.getOwner();
        // auto archTypeDebug = &(*userData->instancePtr).getArchetype();
        // LOGD(" owner : {}, archTypeDebug : {} ", owner->getName(), archTypeDebug->getName())
        auto value = field.getUnsafe<float>(userData->instancePtr);
        LOGD_OLD("value : {}", std::to_string(value))
        userData->node[field.getName()] = value;
    }
    else if (fieldType.match(rfk::getType<bool>()))
    {
        userData->node[field.getName()] = field.getUnsafe<bool>(userData->instancePtr);
    }
    else if (fieldType.match(rfk::getType<char*>()))
    {
        userData->node[field.getName()] = field.getUnsafe<char*>(userData->instancePtr);
    }
    else if (classArcheType != nullptr)
    {
        const rfk::Method* serializeToYaml = classArcheType->getMethodByName("SerializeToYaml", rfk::EMethodFlags::Default, true);
        if (serializeToYaml == nullptr)
        {
            LOGF_OLD("Method SerializeToYaml not found in class {} ?", classArcheType->getName())
        }

        auto target                     = field.getPtrUnsafe(userData->instancePtr);
        userData->node[field.getName()] = serializeToYaml->invokeUnsafe<YAML::Node>(target);
    }
    else
    {
        LOGF_OLD("not support type {}", fieldType.getArchetype()->getName())
    }
    // 此处可以添加对其他类型的支持，例如容器等
    return true;
}
// template <typename T>
YAML::Node Sandbox::ISerializable::SerializeToYaml()
{
    // 在编译时检查T是否继承自ISerializable
    // static_assert(std::is_base_of<ISerializable, T>::value, "T must inherit from ISerializable");

    // 假设object是一个通过Refureku反射得到的实例
    // 对象的类型使用Refureku的rkfq::TypeInfo获取
    const rfk::Class& objectType = this->getArchetype();
    LOGD_OLD("Serialize objectType : {}, fieldsCount : {} ", objectType.getName(), std::to_string(objectType.getFieldsCount()))


    InUserData userData;
    userData.instancePtr = reinterpret_cast<void*>(this);

    objectType.foreachField(SerializeObjectField, &userData, true);

    return userData.node;
}

inline bool DeserializeObjectField(const rfk::Field& field, void* inUserData)
{
    const rfk::Type& fieldType      = field.getType();
    auto             userData       = static_cast<OutUserData*>(inUserData);
    auto             archetype      = fieldType.getArchetype();
    auto             classArcheType = rfk::classCast(archetype);
    LOGD_OLD("Deserialize field : {} , fieldType : {}, isPointer : {}, isValue : {}", field.getName(), archetype->getName(), fieldType.isPointer(), fieldType.isValue())
    try
    {
        // TODO:映射表
        if (fieldType.match(rfk::getType<int>()))
        {
            auto value = userData->node->operator[](field.getName()).as<int>();
            field.setUnsafe(userData->instancePtr, value);
        }
        else if (fieldType.match(rfk::getType<float>()))
        {
            auto value = userData->node->operator[](field.getName()).as<float>();
            LOGD_OLD("value : {}", std::to_string(value))
            field.setUnsafe(userData->instancePtr, value);
        }
        else if (fieldType.match(rfk::getType<bool>()))
        {
            auto value = userData->node->operator[](field.getName()).as<bool>();
            // LOGD("value : {}", std::to_string(value))
            field.setUnsafe(userData->instancePtr, value);
        }
        else if (fieldType.match(rfk::getType<char*>()))  // fixed mapping to 'class String'
        {
            auto               stringClassArcheType = rfk::classCast(field.getOuterEntity());
            const rfk::Method* construct            = stringClassArcheType->getMethodByName("Construct", rfk::EMethodFlags::Default, true);
            auto               value                = userData->node->operator[](field.getName()).as<std::string>();
            construct->invokeUnsafe<void>(reinterpret_cast<void*>(userData->instancePtr), value);
        }
        else if (classArcheType != nullptr)
        {
            const rfk::Method* deserializeFromYaml = classArcheType->getMethodByName("DeserializeFromYaml", rfk::EMethodFlags::Default, true);
            if (deserializeFromYaml == nullptr)
            {
                LOGF_OLD("Method DeserializeFromYaml not found in class {} ?", classArcheType->getName())
            }
            LOGD("Serializable", "Call DeserializeFromYaml Method found in class {}", classArcheType->getName())

            const YAML::Node& subNode = userData->node->operator[](field.getName());
            void*             target  = field.getPtrUnsafe(userData->instancePtr);
            auto              success = deserializeFromYaml->invokeUnsafe<bool>(target, subNode);
            if (!success)
            {
                LOGF_OLD("DeserializeFromYaml failed in field {}", field.getName())
            }
        }
        else
        {
            LOGF_OLD("not support type {}", fieldType.getArchetype()->getName())
        }
    }
    catch (YAML::InvalidNode& e)
    {
        LOGW_OLD("DeserializeFromYaml InvalidNode : {}", e.what())
    }
    // 此处可以添加对其他类型的支持，例如容器等
    return true;
}

// template <typename T>
bool Sandbox::ISerializable::DeserializeFromYaml(const YAML::Node& inNode)
{
    // static_assert(std::is_base_of<ISerializable, T>::value, "T must inherit from ISerializable");

    const rfk::Class& objectType = this->getArchetype();
    LOGD("Serializable", "Deserialize objectType class name {}\n{}", objectType.getName(), GetCallStack())


    OutUserData userData;
    userData.node        = &inNode;
    userData.instancePtr = reinterpret_cast<void*>(this);
    // LOGD("Serializable", "check {} == {}", PtrToHexString(this), PtrToHexString(userData.instancePtr))
    // if (PtrToHexString(this) != PtrToHexString(userData.instancePtr))
    // {
    //     LOGF("Serializable", "static_cast<void*>(this) != userData.instancePtr")
    // }
    objectType.foreachField(DeserializeObjectField, &userData, true);
    return true;
}