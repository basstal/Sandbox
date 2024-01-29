
#include <iostream>
#include <memory>
#include <string>

#include "Application.hpp"
#include "NativeFileSystem.hpp"
int main()
{
	std::string binariesDir = FileSystemBase::getBinariesDir();
	FileSystemBase::addDllSearchPath(binariesDir + "/../ThirdPartyLibs");

	try
	{
		// app.run();
		auto application = std::make_unique<Application>();
		while (!glfwWindowShouldClose(application->surface->glfwWindow))
		{
			glfwPollEvents();
			application->DrawFrame();
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
