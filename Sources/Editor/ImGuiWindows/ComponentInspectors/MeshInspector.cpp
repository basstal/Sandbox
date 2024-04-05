#include "pch.hpp"

#include "MeshInspector.hpp"

#include "Generated/MeshInspector.rfks.h"
Sandbox::MeshInspector::MeshInspector() { name = "Mesh"; }

void Sandbox::MeshInspector::OnInspectorGui()
{
    Inspector::OnInspectorGui();
    ImGui::Text("TODO Mesh Inspector");
}
