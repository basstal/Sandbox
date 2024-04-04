#include "pch.hpp"

#include "TransformInspector.hpp"


Sandbox::TransformInspector::TransformInspector() : currentGizmoOperation(ImGuizmo::TRANSLATE), currentGizmoMode(ImGuizmo::LOCAL), useSnap(false)
{
    name = "TransformInspector";
}

void Sandbox::TransformInspector::OnGui()
{
    // 创建一个子区域/子窗口
    ImGui::BeginChild(name.c_str());
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::EndChild();
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_T))
        currentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E))
        currentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R))  // r Key
        currentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
        currentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
        currentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
        currentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Universal", currentGizmoOperation == ImGuizmo::UNIVERSAL))
        currentGizmoOperation = ImGuizmo::UNIVERSAL;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    // glm::mat4 model = target->transform->GetModelMatrix();
    // float matrix[16];
    // memcpy(matrix, glm::value_ptr(model), sizeof(float) * 16);
    // ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
    auto position = m_target->transform->position;
    memcpy(matrixTranslation, glm::value_ptr(position.ToGlmVec3()), sizeof(float) * 3);
    if (ImGui::InputFloat3("Tr", matrixTranslation))
    {
        position.x                    = matrixTranslation[0];
        position.y                    = matrixTranslation[1];
        position.z                    = matrixTranslation[2];
        m_target->transform->position = position;
    }
    if (ImGui::InputFloat3("Rt", matrixRotation))
    {
        // TODO:
    }
    auto scale = m_target->transform->scale;
    memcpy(matrixScale, glm::value_ptr(scale.ToGlmVec3()), sizeof(float) * 3);
    if (ImGui::InputFloat3("Sc", matrixScale))
    {
        scale.x                    = matrixScale[0];
        scale.y                    = matrixScale[1];
        scale.z                    = matrixScale[2];
        m_target->transform->scale = scale;
    }
    // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

    if (currentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
            currentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
            currentGizmoMode = ImGuizmo::WORLD;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_S))
        useSnap = !useSnap;
    ImGui::Checkbox("##UseSnap", &useSnap);
    ImGui::SameLine();

    switch (currentGizmoOperation)  // NOLINT(clang-diagnostic-switch, clang-diagnostic-switch-enum)
    {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        default:
            break;
    }
    // ImGui::Checkbox("Bound Sizing", &boundSizing);
    // if (boundSizing)
    // {
    // 	ImGui::PushID(3);
    // 	ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
    // 	ImGui::SameLine();
    // 	ImGui::InputFloat3("Snap", boundsSnap);
    // 	ImGui::PopID();
    // }
    ImGui::EndChild(); // 结束子区域/子窗口
}
