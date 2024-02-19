#include "GizmoEditor.hpp"
#include <glm/gtc/type_ptr.hpp> // 包含glm::value_ptr
#include "ImGuizmo.h"
#include "Infrastructures/DataBinding.hpp"

std::shared_ptr<GizmoEditor> GizmoEditor::m_instance = nullptr;

GizmoEditor::GizmoEditor(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	m_applicationEditor = applicationEditor;
}

GizmoEditor::~GizmoEditor()
{
	// TODO:bug fix registeredEditors size 0 when shutdown
	// Unregister();
}

void GizmoEditor::DrawFrame()
{
	// ImGuizmo::BeginFrame();
	// // ImGuiIO& io = ImGui::GetIO();
	// auto identityMatrix = glm::mat4(1.0f);
	// auto cameraView = Application::Instance->editorCamera->GetViewMatrix();
	// auto cameraProjection = Application::Instance->projection;
	//
	// ImGuizmo::SetDrawlist();
	//
	// ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(identityMatrix), 10.0f);
}

void GizmoEditor::Create(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	if (m_instance != nullptr)
	{
		return;
	}
	m_instance = std::make_shared<GizmoEditor>(applicationEditor);
	m_instance->Register(m_instance);
}
