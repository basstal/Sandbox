#include "pch.hpp"

#include "Inspector.hpp"

#include "Editor/IImGuiWindow.hpp"
#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "Generated/Inspector.rfks.h"
#include "Serialization/Property/InspectComponentName.hpp"

std::map<std::string, std::shared_ptr<Sandbox::Inspector>> Sandbox::Inspector::componentNameToInspector = {};

Sandbox::Inspector::Inspector() { name = "Inspector"; }

void Sandbox::Inspector::Prepare()
{
    auto& archetype = getArchetype();
    if (archetype.isSubclassOf(Inspector::staticGetArchetype()))
    {
        auto inspectComponentNameProperty = getArchetype().getProperty<InspectComponentName>();
        name                              = inspectComponentNameProperty != nullptr ? inspectComponentNameProperty->name : "TODO";
        // Inspector 的子类不需要下面的逻辑
        return;
    }
    // IImGuiWindow::Prepare();
    LoadFromFile(GetConfigCachePath());
    std::map<std::string, const rfk::Class*> componentNameToInspectorClass;
    auto                                     inspectorDerivedClasses = Inspector::staticGetArchetype().getDirectSubclasses();

    for (auto derived : inspectorDerivedClasses)
    {
        const rfk::Class*           derivedClass                  = rfk::classCast(derived);
        const InspectComponentName* inspectComponentName          = derivedClass->getProperty<InspectComponentName>();
        componentNameToInspectorClass[inspectComponentName->name] = derivedClass;
    }

    // 将所有 IComponent 的派生类按名字注册到 componentNameToInspector
    auto derivedClasses = IComponent::staticGetArchetype().getDirectSubclasses();
    // LOGD("Test", "derivedClasses count {}", derivedClasses.size())
    for (auto derivedClass : derivedClasses)
    {
        auto componentClass = rfk::classCast(derivedClass);
        auto componentName  = componentClass->getName();
        if (componentNameToInspector.contains(componentName))
        {
            LOGW("Editor", "Conflict found for inspector with class name {}", componentName)
            continue;
        }
        if (!componentNameToInspectorClass.contains(componentName))
        {
            LOGW("Editor", "No inspector found for component with class name {}", componentName)
            continue;
        }
        LOGD("Editor", "Register inspector for class {}", componentName)
        auto sharedInspectorInstance = componentNameToInspectorClass[componentName]->makeSharedInstance<Inspector>();
        sharedInspectorInstance->Prepare();
        componentNameToInspector[componentName] = sharedInspectorInstance;
        // LOGD("Test", "Inspector: {}", derivedClass->getName())
    }
}

void Sandbox::Inspector::OnInspectorGui() {}

void Sandbox::Inspector::OnGui()
{
    if (name == "Inspector")
    {
        DrawComponentInspectors();
        if (target != nullptr)
        {
            // 如果窗口是右键点击，弹出上下文菜单
            if (ImGui::BeginPopupContextWindow())
            {
                // 菜单项
                if (ImGui::MenuItem("Create/Mesh"))
                {
                    target->AddComponent<Mesh>();
                }
                if (ImGui::MenuItem("Create/Camera"))
                {
                    target->AddComponent<Camera>();
                }
                ImGui::EndPopup();
            }
        }
    }
    else
    {
        // 创建一个子区域/子窗口
        auto childName  = name.c_str();
        auto childFlags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;
        ImGui::BeginChild(childName, ImVec2(0, 0), childFlags);
        if (!ImGui::CollapsingHeader(childName, &isComponentExist, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::EndChild();
            return;
        }
        OnInspectorGui();
        ImGui::EndChild();
        ImGui::Separator();
        ImGui::NewLine();
    }
}

std::vector<std::shared_ptr<Sandbox::IComponent>> toBeRemoved;

void Sandbox::Inspector::DrawComponentInspectors()
{
    if (target == nullptr)
    {
        return;
    }
    auto components = target->GetComponents();
    toBeRemoved.clear();
    for (auto component : components)
    {
        std::string className = component->GetDerivedClassName();
        if (!componentNameToInspector.contains(className))
        {
            LOGW("Editor", "No inspector found for component with class name {}", className)
            continue;
        }
        auto inspector = componentNameToInspector[className];
        if (!inspector->isComponentExist)
        {
            toBeRemoved.push_back(component);
            inspector->isComponentExist = true;
        }
        else
        {
            inspector->OnGui();
        }
    }
    for (auto& toBeRemovedComponent : toBeRemoved)
    {
        target->RemoveComponent(toBeRemovedComponent);
    }
}

void Sandbox::Inspector::InspectTarget(std::shared_ptr<GameObject> inTarget)
{
    target = inTarget;
    if (name == "Inspector" && target != nullptr)  // If the inspector is the main inspector
    {
        for (auto& pair : componentNameToInspector)
        {
            if (pair.second == nullptr)
            {
                continue;
            }
            pair.second->InspectTarget(inTarget);
        }
    }
}
