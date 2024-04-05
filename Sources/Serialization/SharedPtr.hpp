#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>

#include "FileSystem/Logger.hpp"
#include "Generated/SharedPtr.rfkh.h"
#include "Refureku/Object.h"
#include "Refureku/TypeInfo/Archetypes/Struct.h"
#include "Refureku/TypeInfo/Database.h"
#include "Refureku/TypeInfo/Entity/EntityCast.h"
#include "Refureku/TypeInfo/Functions/Method.h"
#include "Refureku/TypeInfo/Type.h"
#include "Refureku/TypeInfo/Variables/Field.h"
#include "Refureku/TypeInfo/Variables/Variable.h"

namespace Sandbox NAMESPACE()
{
    template <typename T>
    class CLASS() SharedPtr : public std::shared_ptr<T>, public rfk::Object
    {
    public:
        // 使用using引入std::shared_ptr<T>的构造函数
        using std::shared_ptr<T>::shared_ptr;

        // 添加一个接受std::shared_ptr<T>的构造函数
        SharedPtr(const std::shared_ptr<T>& other) : std::shared_ptr<T>(other) {}

        // 如果需要，还可以添加一个接受std::shared_ptr<T>&&的构造函数（移动构造）
        SharedPtr(std::shared_ptr<T>&& other) : std::shared_ptr<T>(std::move(other)) {}

        std::shared_ptr<T> ToStdSharedPtr() const { return *this; }

        METHOD()
        YAML::Node SerializeToYaml();

        METHOD()
        bool DeserializeFromYaml(const YAML::Node& node);

        const rfk::Struct& getArchetype() const noexcept override;

        Sandbox_SharedPtr_GENERATED
    };

    template <typename T>
    YAML::Node Sandbox::SharedPtr<T>::SerializeToYaml()
    {
        const rfk::Class& archeType = T::staticGetArchetype();
        LOGD_OLD("[SharedPtr] objectType : {}, fieldsCount : {} ", archeType.getName(), std::to_string(archeType.getFieldsCount()))

        // YAML::Node result;
        // for (auto it = this->begin(); it != this->end(); ++it)
        // {
        //     YAML::Node elementNode = it->SerializeToYaml();
        //     result.push_back(elementNode);
        // }
        return this->get()->SerializeToYaml();
        // rfk::Method const *serializeToYaml = archeType.getMethodByName("SerializeToYaml", rfk::EMethodFlags::Default, true);
        // if (serializeToYaml == nullptr)
        // {
        //     LOGF("[SharedPtr] Method SerializeToYaml not found in class {} ?", archeType.getName())
        // }

        // return serializeToYaml->invokeUnsafe<YAML::Node>(reinterpret_cast<void *>(this->get()));
    }

    template <typename T>
    bool Sandbox::SharedPtr<T>::DeserializeFromYaml(const YAML::Node& node)
    {
        const rfk::Class& archeType = T::staticGetArchetype();
        LOGD_OLD("[SharedPtr] objectType : {}, fieldsCount : {} ", archeType.getName(), std::to_string(archeType.getFieldsCount()))
        this->get()->DeserializeFromYaml(node);
        return true;
    }

    template <typename T>
    const rfk::Struct& SharedPtr<T>::getArchetype() const noexcept
    {
        LOGF("Core", "SharedPtr<T>::getArchetype Not implemented");  // 为了避免头文件中的循环引用，而且暂时不需要根据 shared ptr 获取 T 的 原型信息，所以这里直接抛出异常
		return T::staticGetArchetype();
	}
} // namespace Sandbox NAMESPACE()

File_SharedPtr_GENERATED
