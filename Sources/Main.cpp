#include <iostream>
#include <memory>

#include "Editor/ApplicationEditor.hpp"
#include "Editor/GizmoEditor.hpp"
#include "Editor/OverlayEditor.hpp"
#include "Editor/SettingsEditor.hpp"
#include "Editor/ValueEditor.hpp"
#include "Infrastructures/DataBinding.hpp"
#include "Rendering/Application.hpp"
#include "Rendering/Settings.hpp"

int main()
{
	// std::string binariesDir = FileSystemBase::getBinariesDir();
	// FileSystemBase::addDllSearchPath(binariesDir + "/../ThirdPartyLibs");
	try
	{
		auto setting = std::make_shared<Settings>();
		DataBinding::Create("Rendering/Settings", setting);

		Application::Instance = std::make_unique<Application>(setting);
		Application::Instance->Initialize();
		std::shared_ptr<ApplicationEditor> applicationEditor = std::make_shared<ApplicationEditor>(Application::Instance);
		SettingsEditor::Create(applicationEditor);
		ValueEditor::Create(applicationEditor);
		GizmoEditor::Create(applicationEditor);
		OverlayEditor::Create(applicationEditor);
		while (!glfwWindowShouldClose(Application::Instance->surface->glfwWindow))
		{
			glfwPollEvents();
			Application::Instance->DrawFrame(applicationEditor);
		}
		vkDeviceWaitIdle(Application::Instance->device->vkDevice);
		applicationEditor->Cleanup();
		Application::Instance->Cleanup();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
