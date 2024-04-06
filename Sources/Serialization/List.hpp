#pragma once

#include <yaml-cpp/yaml.h>

#include "FileSystem/Logger.hpp"
#include "Generated/List.rfkh.h"

namespace Sandbox NAMESPACE()
{
    template <typename T>
    class CLASS() List : public std::vector<T>, public rfk::Object
    {
    public:
        METHOD()
        YAML::Node SerializeToYaml();

        METHOD()
        bool DeserializeFromYaml(const YAML::Node& node);

        // const rfk::Struct& getArchetype() const noexcept override;

        Sandbox_List_GENERATED
    };

    template <typename T>
    YAML::Node Sandbox::List<T>::SerializeToYaml()
    {
        const rfk::Class& objectType = T::staticGetArchetype();
        LOGD_OLD("[List] objectType : {} ", objectType.getName())

        // 将 vector 中每个元素按顺序存放到 YAML::Sequence 中
        YAML::Node result;
        for (auto it = this->begin(); it != this->end(); ++it)
        {
            YAML::Node elementNode = it->SerializeToYaml();
            result.push_back(elementNode);
        }
        return result;
    }

    template <typename T>
    bool Sandbox::List<T>::DeserializeFromYaml(const YAML::Node& node)
    {
        const rfk::Class& objectType = T::staticGetArchetype();
        LOGD_OLD("[List] Deserialize objectType : {} ", objectType.getName())

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            T elementNode;
            elementNode.DeserializeFromYaml(*it);
            this->push_back(elementNode);
        }
        return true;
    }

    // template <typename T>
    // const rfk::Struct& List<T>::getArchetype() const noexcept
    // {
    //     return T::staticGetArchetype();
    // }
}  // namespace Sandbox NAMESPACE()

File_List_GENERATED
