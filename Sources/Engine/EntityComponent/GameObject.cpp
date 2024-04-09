#include "pch.hpp"

#include "GameObject.hpp"

#include "Components/Transform.hpp"
#include "Generated/GameObject.rfks.h"
#include "IComponent.hpp"
#include "Misc/Debug.hpp"

Sandbox::GameObject::GameObject() {}  // transform = AddComponent<Transform>(); }

Sandbox::GameObject::~GameObject() { Cleanup(); }

void Sandbox::GameObject::Cleanup()
{
    if (m_cleaned)
    {
        return;
    }
    for (auto& component : m_components)
    {
        component->Cleanup();
    }
    m_cleaned = true;
}

void Sandbox::GameObject::AddComponent(std::shared_ptr<IComponent> component) { m_components.push_back(component); }

Sandbox::List<Sandbox::SharedPtr<Sandbox::IComponent>> Sandbox::GameObject::GetComponents() { return m_components; }

YAML::Node Sandbox::GameObject::SerializeToYaml()
{
    const rfk::Class& objectType = this->getArchetype();
    LOGD_OLD("Serialize objectType : {}, fieldsCount : {} ", objectType.getName(), std::to_string(objectType.getFieldsCount()))


    InUserData userData;
    userData.instancePtr = reinterpret_cast<void*>(this);
    SerializeObjectField(*objectType.getFieldByName("name"), &userData);
    YAML::Node sequence;
    // 得到每个 component 的派生类，然后再序列化所有字段
    for (auto& component : m_components)
    {
        InUserData componentUserData;
        componentUserData.instancePtr       = reinterpret_cast<void*>(component.get());
        auto derivedClass                   = component->GetDerivedClass();
        componentUserData.node["__Class__"] = derivedClass->getId();
        LOGD("Serializable", "Serialize objectType class name {}", derivedClass->getName())
        derivedClass->foreachField(SerializeObjectField, &componentUserData, true);
        sequence.push_back(componentUserData.node);
    }
    userData.node["m_components"] = sequence;
    return userData.node;
}

bool Sandbox::GameObject::DeserializeFromYaml(const YAML::Node& inNode)
{
    // static_assert(std::is_base_of<ISerializable, T>::value, "T must inherit from ISerializable");

    const rfk::Class& objectType = this->getArchetype();
    LOGD("Serializable", "Deserialize objectType class name {}\n{}", objectType.getName(), GetCallStack())


    OutUserData userData;
    userData.node        = &inNode;
    userData.instancePtr = reinterpret_cast<void*>(this);
    DeserializeObjectField(*objectType.getFieldByName("name"), &userData);
    auto componentsNode = inNode["m_components"];
    if (!componentsNode.IsSequence())
    {
        LOGF("Engine", "Deserialize GameObject failed, m_components is not a sequence!")
    }
    // 得到每个 component 的派生类，然后再反序列化所有字段
    for (size_t i = 0; i < componentsNode.size(); ++i)
    {
        OutUserData componentUserData;
        auto        componentNode     = componentsNode[i];
        componentUserData.node        = &componentNode;
        auto classId                  = componentNode["__Class__"].as<size_t>();
        auto derivedClass             = rfk::getDatabase().getClassById(classId);
        auto instance                 = derivedClass->makeSharedInstance<IComponent>();
        componentUserData.instancePtr = reinterpret_cast<void*>(instance.get());
        LOGD("Serializable", "Deserialize objectType class name {}", derivedClass->getName())
        derivedClass->foreachField(DeserializeObjectField, &componentUserData, true);
        IComponent::onComponentCreate.Trigger(instance);
        instance->gameObject = weak_from_this();
        m_components.push_back(instance);
    }
    return true;
}
