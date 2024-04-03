#include "pch.hpp"

#include "Editor/IImGuiWindow.hpp"
#include "ComponentInspectors/TransformInspector.hpp"
#include "Inspector.hpp"

static std::map<std::string, std::shared_ptr<Sandbox::IImGuiWindow>> componentMapping = {};

Sandbox::Inspector::Inspector()
{
    name = "Inspector";
    componentMapping["Transform"] = std::make_shared<TransformInspector>();
}

void Sandbox::Inspector::OnGui()
{
    ImGui::Begin(name.c_str());
    if (target)
    {
        auto components = target->GetComponents();
        for (auto component : components)
        {
            // TODO: Replace with a more robust way of checking the type of the component
            if (std::dynamic_pointer_cast<Transform>(component))
            {
                componentMapping["Transform"]->OnGui();
            }
            ImGui::Separator();
        }
    }
    ImGui::End();
}