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
        void LoadFromFile(const File& file);

        void SaveToFile(const File& file);

        METHOD()
        YAML::Node SerializeToYaml();

        METHOD()
        bool DeserializeFromYaml(const YAML::Node& node);

        // const rfk::Struct& getArchetype() const noexcept override;

        Sandbox_ISerializable_GENERATED
    };


    // template <typename T>
    // const rfk::Struct& ISerializable<T>::getArchetype() const noexcept { return T::staticGetArchetype(); }
}  // namespace Sandbox NAMESPACE()

File_ISerializable_GENERATED
