#include "ValueEditor.hpp"

#include "Infrastructures/SingletonOrganizer.hpp"

std::shared_ptr<ValueEditor> ValueEditor::m_instance = nullptr;

void ValueEditor::DisplayMatrixInImGui(glm::mat4& mat, const std::string& matrixName)
{
	float imguiMat[4][4];
	ConvertGlmMat4ToImGui(mat, imguiMat);

	ImGui::Text(matrixName.c_str());
	// 你可以使用ImGui的函数来显示或编辑矩阵，例如:
	ImGui::InputFloat4("Row 1", imguiMat[0]);
	ImGui::InputFloat4("Row 2", imguiMat[1]);
	ImGui::InputFloat4("Row 3", imguiMat[2]);
	ImGui::InputFloat4("Row 4", imguiMat[3]);
	// TODO:解决 debugUBO 仅显示用不能编辑问题
	// ConvertImGuiToGlmMat4(imguiMat, mat);
}

void ValueEditor::DrawFrame()
{
	// ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("DebugValues"); // Create a window called "Hello, world!" and append into it.
	// DisplayMatrixInImGui(SingletonOrganizer::Get<Renderer>()->debugUBO.model, "model");
	// DisplayMatrixInImGui(SingletonOrganizer::Get<Renderer>()->debugUBO.view, "view");
	// DisplayMatrixInImGui(SingletonOrganizer::Get<Renderer>()->debugUBO.proj, "proj");

	if (ImGui::Button("Reset"))
	{
		m_applicationEditor->editorCamera->Reset();
		m_applicationEditor->transformGizmo->referenceGameObject->transform->GetPosition() = glm::vec3(0.0f);
		m_applicationEditor->transformGizmo->modelMatrix = glm::mat4(1.0f);
	}
	ImGui::End();
}

void ValueEditor::Create(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	if (m_instance != nullptr)
	{
		return;
	}
	m_instance = std::make_shared<ValueEditor>(applicationEditor);
	m_instance->Register(m_instance);
}
