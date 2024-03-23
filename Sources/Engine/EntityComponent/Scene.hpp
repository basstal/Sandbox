#pragma once
#include <memory>
#include <vector>

namespace Sandbox
{
    class GameObject;

    class Scene
    {
    public:
        std::vector<std::shared_ptr<GameObject>> rootGameObjects;

        static std::shared_ptr<Scene> currentScene;

        static std::shared_ptr<Scene> GetCurrentScene();
    };
}
