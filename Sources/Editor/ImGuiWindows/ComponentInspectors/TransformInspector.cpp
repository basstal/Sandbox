#include "pch.hpp"

#include "TransformInspector.hpp"

#include "Generated/TransformInspector.rfks.h"


void Sandbox::TransformInspector::OnInspectorGui()
{
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    // glm::mat4 model = target->transform->GetModelMatrix();
    // float matrix[16];
    // memcpy(matrix, glm::value_ptr(model), sizeof(float) * 16);
    // ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
    auto position = target->transform->position;
    memcpy(matrixTranslation, glm::value_ptr(position.ToGlmVec3()), sizeof(float) * 3);
    if (ImGui::InputFloat3("Position", matrixTranslation))
    {
        position.x                  = matrixTranslation[0];
        position.y                  = matrixTranslation[1];
        position.z                  = matrixTranslation[2];
        target->transform->position = position;
    }
    auto      rotation     = target->transform->rotation;
    glm::vec3 eulerDegrees = rotation.GetEulerDegrees();
    memcpy(matrixRotation, glm::value_ptr(eulerDegrees), sizeof(float) * 3);
    if (ImGui::InputFloat3("Rotation", matrixRotation))
    {
        eulerDegrees.x              = matrixRotation[0];
        eulerDegrees.y              = matrixRotation[1];
        eulerDegrees.z              = matrixRotation[2];
        target->transform->rotation = glm::quat(glm::radians(eulerDegrees));
    }
    auto scale = target->transform->scale;
    memcpy(matrixScale, glm::value_ptr(scale.ToGlmVec3()), sizeof(float) * 3);
    if (ImGui::InputFloat3("Scale", matrixScale))
    {
        scale.x                  = matrixScale[0];
        scale.y                  = matrixScale[1];
        scale.z                  = matrixScale[2];
        target->transform->scale = scale;
    }
    // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);


    // ImGui::Checkbox("Bound Sizing", &boundSizing);
    // if (boundSizing)
    // {
    // 	ImGui::PushID(3);
    // 	ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
    // 	ImGui::SameLine();
    // 	ImGui::InputFloat3("Snap", boundsSnap);
    // 	ImGui::PopID();
    // }
}
