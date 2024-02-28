#include <iostream>

#include "GameCore/Engine.hpp"

int main()
{
    try
    {
        Engine engine;
        engine.Initialize();
        engine.MainLoop();
        engine.Cleanup();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
