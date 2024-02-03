#include <iostream>
#include <memory>

#include "Editor/ApplicationEditor.hpp"
#include "Rendering/Application.hpp"
#include "Rendering/Settings.hpp"
int main()
{
	// std::string binariesDir = FileSystemBase::getBinariesDir();
	// FileSystemBase::addDllSearchPath(binariesDir + "/../ThirdPartyLibs");
	try
	{
		auto setting = std::make_shared<Settings>();
		Application::Instance = std::make_unique<Application>(setting);
		auto applicationEditor = std::make_unique<ApplicationEditor>(Application::Instance);
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
