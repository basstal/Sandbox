#include "pch.hpp"

#include "Inspector.hpp"

#include "Editor/IImGuiWindow.hpp"
#include "Generated/Inspector.rfks.h"
#include "Serialization/Property/InspectComponentName.hpp"

std::map<std::string, std::shared_ptr<Sandbox::Inspector>> Sandbox::Inspector::componentNameToInspector = {};

Sandbox::Inspector::Inspector() { name = "Inspector"; }

void Sandbox::Inspector::Prepare()
{
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
        auto sharedInspectorInstance            = componentNameToInspectorClass[componentName]->makeSharedInstance<Inspector>();
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
    }
    else
    {
        // 创建一个子区域/子窗口
        auto childName  = name.c_str();
        auto childFlags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;
        ImGui::BeginChild(childName, ImVec2(0, 0), childFlags);
        if (!ImGui::CollapsingHeader(childName, ImGuiTreeNodeFlags_DefaultOpen))
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

void Sandbox::Inspector::DrawComponentInspectors()
{
    if (target == nullptr)
    {
        return;
    }
    auto components = target->GetComponents();
    for (auto component : components)
    {
        std::string className = component->GetDerivedClassName();
        if (!componentNameToInspector.contains(className))
        {
            LOGW("Editor", "No inspector found for component with class name {}", className)
            continue;
        }
        auto inspector = componentNameToInspector[className];
        inspector->OnGui();
    }
}

void Sandbox::Inspector::InspectTarget(std::shared_ptr<GameObject> inTarget)
{
    target = inTarget;
    if (name == "Inspector")  // If the inspector is the main inspector
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
