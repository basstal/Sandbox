#include "pch.hpp"

#include "MeshInspector.hpp"

#include "Editor/ImGuiWidgets/TreeView.hpp"
#include "Editor/ImGuiWindows/ContentBrowser.hpp"
#include "Editor/ImGuiWindows/Hierarchy.hpp"
#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "FileSystem/Directory.hpp"
#include "Generated/MeshInspector.rfks.h"

void Sandbox::MeshInspector::OnInspectorGui()
{
    Inspector::OnInspectorGui();

    auto mesh      = target->GetComponent<Mesh>();
    auto modelPath = mesh->GetModelPath();
    ImGui::InputText("ModelPath", &modelPath);
    const std::string extensions[] = {".fbx", ".obj", ".gltf"};
    DragAndDropContentBrowser(extensions, 3, modelPath);
    mesh->SetModelPath(modelPath);
}
