
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
		auto application = std::make_unique<Application>();
		auto applicationEditor = std::make_unique<ApplicationEditor>(application);
		while (!glfwWindowShouldClose(application->surface->glfwWindow))
		{
			glfwPollEvents();
			application->DrawFrame(applicationEditor);
		}
		vkDeviceWaitIdle(application->device->vkDevice);
		application->Cleanup();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
