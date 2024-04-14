#include "pch.hpp"

#include "MeshInspector.hpp"

#include "Engine/EntityComponent/Components/Mesh.hpp"
#include "FileSystem/Directory.hpp"
#include "Generated/MeshInspector.rfks.h"

void Sandbox::MeshInspector::OnInspectorGui()
{
    Inspector::OnInspectorGui();

    auto mesh      = target->GetComponent<Mesh>();
    auto modelPath = mesh->GetModelPath();
    ImGui::InputText("ModelPath", &modelPath);
    mesh->SetModelPath(modelPath);
}
