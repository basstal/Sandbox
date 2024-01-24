#include "Graphics.hpp"

int main()
{
	std::string binariesDir = FileSystemBase::getBinariesDir();
	FileSystemBase::addDllSearchPath(binariesDir + "/../ThirdPartyLibs");

	HelloTriangleApplication app = HelloTriangleApplication();

	try
	{
		app.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}