#pragma once

#include <yaml-cpp/yaml.h>

#include "FileSystem/File.hpp"
#include "FileSystem/Logger.hpp"
#include "Generated/ISerializable.rfkh.h"

namespace Sandbox NAMESPACE()
{

    // template <typename T>
    class CLASS() ISerializable : public rfk::Object
    {
    public:
        struct InUserData
        {
            YAML::Node node;
            void*      instancePtr;
        };

        struct OutUserData
        {
            const YAML::Node* node;
            void*             instancePtr;
        };
        // TODO:改个更像序列化的名字
        void LoadFromFile(const File& file);
        // TODO:改个更像序列化的名字
        void SaveToFile(const File& file);

        METHOD()
        virtual YAML::Node SerializeToYaml();

        METHOD()
        virtual bool DeserializeFromYaml(const YAML::Node& inNode);

        // const rfk::Struct& getArchetype() const noexcept override;

        Sandbox_ISerializable_GENERATED
    };

    bool DeserializeObjectField(const rfk::Field& field, void* inUserData);

    bool SerializeObjectField(const rfk::Field& field, void* inUserData);

    // template <typename T>
    // const rfk::Struct& ISerializable<T>::getArchetype() const noexcept { return T::staticGetArchetype(); }
}  // namespace Sandbox NAMESPACE()

File_ISerializable_GENERATED
