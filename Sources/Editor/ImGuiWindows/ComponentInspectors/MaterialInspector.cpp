#include "pch.hpp"

#include "MaterialInspector.hpp"

#include "Editor/ImGuiWindows/ContentBrowser.hpp"
#include "Engine/EntityComponent/Components/Material.hpp"
#include "Engine/EntityComponent/Scene.hpp"
#include "Generated/MaterialInspector.rfks.h"
void Sandbox::MaterialInspector::OnInspectorGui()
{
    Inspector::OnInspectorGui();
    auto material = target->GetComponent<Material>();
    auto changed  = InspectorProperty(material->albedoPath, "albedo");
    changed       = changed || InspectorProperty(material->metallicPath, "metallic");
    changed       = changed || InspectorProperty(material->roughnessPath, "roughness");
    changed       = changed || InspectorProperty(material->aoPath, "ao");
    if (changed)
    {
        material->LoadImages(renderer);
        Scene::currentScene->isRenderMeshesDirty = true;
    }
}


bool Sandbox::MaterialInspector::InspectorProperty(String& property, const std::string& label)
{
    auto changed          = false;
    auto propertyFilePath = property.ToStdString();
    ImGui::InputText(label.c_str(), &propertyFilePath);
    const std::string extensions[] = {".png", ".jpeg", ".jpg", ".ktx"};
    DragAndDropContentBrowser(extensions, 4, propertyFilePath);
    if (property.ToStdString() != propertyFilePath)
    {
        changed = true;
    }
    property = propertyFilePath;
    return changed;
}
