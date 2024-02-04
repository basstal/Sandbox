#include "SettingsEditor.hpp"

#include "Infrastructures/DataBinding.hpp"

std::shared_ptr<SettingsEditor> SettingsEditor::m_instance = nullptr;

SettingsEditor::SettingsEditor(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	m_applicationEditor = applicationEditor;
}

SettingsEditor::~SettingsEditor()
{
	// TODO:bug fix registeredEditors size 0 when shutdown
	// Unregister();
}

void SettingsEditor::DrawFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		std::shared_ptr<TDataBinding<std::shared_ptr<Settings>>> settingsBinding = std::dynamic_pointer_cast<TDataBinding<std::shared_ptr<Settings>>>(DataBinding::Get("Rendering/Settings"));
		std::shared_ptr<Settings> settings = settingsBinding->GetData();

		ImGui::Begin("Rendering/Settings"); // Create a window called "Hello, world!" and append into it.

		// ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &m_applicationEditor->show_demo_window); // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &m_applicationEditor->show_another_window);

		// ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&m_applicationEditor->clear_color); // Edit 3 floats representing a color

		// if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
		// 	counter++;
		// ImGui::SameLine();
		// ImGui::Text("counter = %d", counter);
		auto changed = ImGui::Checkbox("IsWindow", &settings->IsWindow);
		if (settings->IsWindow)
		{
			changed = ImGui::DragInt("Width", &settings->Width) || changed;
			changed = ImGui::DragInt("Height", &settings->Height) || changed;
		}
		if (changed)
		{
			settingsBinding->Trigger();
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}
}

void SettingsEditor::Create(std::shared_ptr<ApplicationEditor> applicationEditor)
{
	if (m_instance != nullptr)
	{
		return;
	}
	m_instance = std::make_shared<SettingsEditor>(applicationEditor);
	m_instance->Register(m_instance);
}
