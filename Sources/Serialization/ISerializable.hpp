#pragma once

#include <yaml-cpp/yaml.h>

#include "Generated/ISerializable.rfkh.h"
#include "FileSystem/File.hpp"
#include "FileSystem/Logger.hpp"
#include "Refureku/Object.h"
#include "Refureku/TypeInfo/Type.h"
#include "Refureku/TypeInfo/Archetypes/Struct.h"
#include "Refureku/TypeInfo/Entity/EntityCast.h"
#include "Refureku/TypeInfo/Variables/Field.h"
#include "Refureku/TypeInfo/Database.h"
#include "Refureku/TypeInfo/Functions/Method.h"
#include "Refureku/TypeInfo/Variables/Variable.h"

namespace Sandbox
NAMESPACE()
{
    template <typename T>
    class CLASS() ISerializable : public rfk::Object
    {
    public:
        void LoadFromFile(const File& file);

        void SaveToFile(const File& file);

        METHOD()

        YAML::Node SerializeToYaml();

        METHOD()

        bool DeserializeFromYaml(const YAML::Node& node);

        rfk::Struct const &getArchetype() const noexcept override;

        Sandbox_ISerializable_GENERATED
    };

    template <typename T>
    void ISerializable<T>::LoadFromFile(const File& file)
    {
        if (!file.Exists())
        {
            LOGW("Cant LoadFromFile : file {} not exists", file.path.string())
            return;
        }
        auto node = YAML::LoadFile(file.path.string());
        auto success = DeserializeFromYaml(node);
        assert(success && "DescrializeFromYaml failed");
    }

    template <typename T>
    void Sandbox::ISerializable<T>::SaveToFile(const File& file)
    {
        if (!file.Exists())
        {
            file.CreateDirectory();
        }
        auto node = SerializeToYaml();
        std::ofstream fout(file.path);
        fout << node;
    }

    template <typename T>
    YAML::Node Sandbox::ISerializable<T>::SerializeToYaml()
    {
        // 在编译时检查T是否继承自ISerializable
        static_assert(std::is_base_of<ISerializable, T>::value, "T must inherit from ISerializable");


        // 假设object是一个通过Refureku反射得到的实例
        // 对象的类型使用Refureku的rkfq::TypeInfo获取
        const rfk::Class& objectType = T::staticGetArchetype();
        LOGD("objectType : {} ", objectType.getName())

        struct UserData
        {
            YAML::Node node;
            T* instancePtr;
        };
        UserData userData;
        userData.instancePtr = static_cast<T*>(this);
        objectType.foreachField([](rfk::Field const& field, void* inUserData)
        {
            const rfk::Type& fieldType = field.getType();
            auto archeType = fieldType.getArchetype();
            const rfk::Class* classArcheType = rfk::classCast(archeType);
            LOGD("field : {} , fieldType : {}, isPointer : {}, isValue : {}", field.getName(), archeType->getName(), fieldType.isPointer(), fieldType.isValue())
            auto userData = static_cast<UserData*>(inUserData);
            // TODO:映射表
            if (fieldType.match(rfk::getType<int>()))
            {
                userData->node[field.getName()] = field.get<int, T>(*userData->instancePtr);
            }
            else if (fieldType.match(rfk::getType<float>()))
            {
                // auto owner = field.getOwner();
                // auto archTypeDebug = &(*userData->instancePtr).getArchetype();
                // LOGD(" owner : {}, archTypeDebug : {} ", owner->getName(), archTypeDebug->getName())
                auto value = field.get<float, T>(*userData->instancePtr);
                LOGD("value : {}", std::to_string(value))
                userData->node[field.getName()] = value;
            }
            else if (fieldType.match(rfk::getType<bool>()))
            {
                userData->node[field.getName()] = field.get<bool, T>(*userData->instancePtr);
            }
            else if (classArcheType != nullptr)
            {
                rfk::Method const* serializeToYaml = classArcheType->getMethodByName("SerializeToYaml", rfk::EMethodFlags::Default, true);
                if (serializeToYaml == nullptr)
                {
                    LOGF("Method SerializeToYaml not found in class {} ?", classArcheType->getName())
                }

                auto target = field.getPtr(*userData->instancePtr);
                userData->node[field.getName()] = serializeToYaml->invokeUnsafe<YAML::Node>(target);
            }
            else
            {
                LOGF("not support type {}", fieldType.getArchetype()->getName())
            }
            // 此处可以添加对其他类型的支持，例如容器等
            return true;
        }, &userData);


        return userData.node;
    }

    template <typename T>
    bool ISerializable<T>::DeserializeFromYaml(const YAML::Node& inNode)
    {
        static_assert(std::is_base_of<ISerializable, T>::value, "T must inherit from ISerializable");

        const rfk::Class& objectType = T::staticGetArchetype();
        LOGD("Descrialize objectType : {} ", objectType.getName())

        struct UserData
        {
            const YAML::Node* node;
            T* instancePtr;
        };
        UserData userData;
        userData.node = &inNode;
        userData.instancePtr = static_cast<T*>(this);
        objectType.foreachField([](rfk::Field const& field, void* inUserData)
        {
            const rfk::Type& fieldType = field.getType();
            auto userData = static_cast<UserData*>(inUserData);
            auto archeType = fieldType.getArchetype();
            auto classArcheType = rfk::classCast(archeType);
            LOGD("Descrialize field : {} , fieldType : {}, isPointer : {}, isValue : {}", field.getName(), archeType->getName(), fieldType.isPointer(), fieldType.isValue())
            // TODO:映射表
            if (fieldType.match(rfk::getType<int>()))
            {
                auto value = userData->node->operator[](field.getName()).as<int>();
                field.set(*userData->instancePtr, value);
            }
            else if (fieldType.match(rfk::getType<float>()))
            {
                auto value = userData->node->operator[](field.getName()).as<float>();
                LOGD("value : {}", std::to_string(value))
                field.set(*userData->instancePtr, value);
            }
            else if (fieldType.match(rfk::getType<bool>()))
            {
                auto value = userData->node->operator[](field.getName()).as<bool>();
                // LOGD("value : {}", std::to_string(value))
                field.set(*userData->instancePtr, value);
            }
            else if (classArcheType != nullptr)
            {
                rfk::Method const* deserializeFromYaml = classArcheType->getMethodByName("DeserializeFromYaml", rfk::EMethodFlags::Default, true);
                if (deserializeFromYaml == nullptr)
                {
                    LOGF("Method DeserializeFromYaml not found in class {} ?", classArcheType->getName())
                }
                const YAML::Node& subNode = userData->node->operator[](field.getName());
                auto target = field.getPtr(*userData->instancePtr);
                auto success = deserializeFromYaml->invokeUnsafe<bool>(target, subNode);
                if (!success)
                {
                    LOGF("DeserializeFromYaml failed in field {}", field.getName())
                }
            }
            else
            {
                LOGF("not support type {}", fieldType.getArchetype()->getName())
            }
            // 此处可以添加对其他类型的支持，例如容器等
            return true;
        }, &userData);
        return true;
    }

    template <typename T>
    rfk::Struct const &ISerializable<T>::getArchetype() const noexcept
    {
        return T::staticGetArchetype();
    }
}

File_ISerializable_GENERATED
