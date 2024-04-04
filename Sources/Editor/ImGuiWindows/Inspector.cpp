#include "pch.hpp"

#include "Inspector.hpp"

#include "ComponentInspectors/TransformInspector.hpp"
#include "Editor/IImGuiWindow.hpp"

std::map<std::string, std::shared_ptr<Sandbox::Inspector>> Sandbox::Inspector::ComponentMapping = {{"Transform", std::make_shared<Sandbox::TransformInspector>()}};

Sandbox::Inspector::Inspector() { name = "Inspector"; }

void Sandbox::Inspector::OnGui()
{
    ImGui::Begin(name.c_str());
    if (m_target)
    {
        auto components = m_target->GetComponents();
        for (auto component : components)
        {
            // TODO: Replace with a more robust way of checking the type of the component
            if (std::dynamic_pointer_cast<Transform>(component))
            {
                ComponentMapping["Transform"]->OnGui();
            }
            ImGui::Separator();
        }
    }
    ImGui::End();
}
void Sandbox::Inspector::InspectTarget(std::shared_ptr<GameObject> inTarget)
{
    m_target = inTarget;
    if (name == "Inspector")  // If the inspector is the main inspector
    {
        for (auto& pair : ComponentMapping)
        {
            pair.second->InspectTarget(inTarget);
        }
    }
}
