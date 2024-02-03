#include <iostream>
#include <memory>

#include "Editor/ApplicationEditor.hpp"
#include "Rendering/Application.hpp"
int main()
{
	// std::string binariesDir = FileSystemBase::getBinariesDir();
	// FileSystemBase::addDllSearchPath(binariesDir + "/../ThirdPartyLibs");

	try
	{
		Application::Instance = std::make_unique<Application>();
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
