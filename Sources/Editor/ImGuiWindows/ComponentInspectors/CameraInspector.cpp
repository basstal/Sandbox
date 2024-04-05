#include "pch.hpp"

#include "CameraInspector.hpp"

#include "Engine/EntityComponent/Components/Camera.hpp"
#include "Generated/CameraInspector.rfks.h"

Sandbox::CameraInspector::CameraInspector() { name = "Camera"; }

void Sandbox::CameraInspector::OnInspectorGui()
{
    auto camera = m_target->GetComponent<Camera>();

    auto rotationChanged = ImGui::InputFloat("rotationX", &camera->rotationX, 3.f, 18.0f, "%.3f");
    rotationChanged      = ImGui::InputFloat("rotationZ", &camera->rotationZ, 3.f, 18.0f, "%.3f") || rotationChanged;
    if (rotationChanged)
    {
        camera->UpdateCameraVectors();
    }
    ImGui::InputFloat("fov", &camera->fieldOfView, 0.5f, 3.0f, "%.3f");
    ImGui::InputFloat("aspectRatio", &camera->aspectRatio, 0.02f, 0.12f, "%.3f");

    ImGui::InputFloat("nearPlane", &camera->nearPlane, 0.1f, 1.0f, "%.3f");
    ImGui::InputFloat("farPlane", &camera->farPlane, 0.1f, 1.0f, "%.3f");

    // DrawFieldsReflected<Camera>(camera);
}
